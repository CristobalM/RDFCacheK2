//
// Created by cristobal on 9/6/21.
//

#include <K2TreeBulkOp.hpp>
#include <UpdatesLogger.hpp>
#include <gtest/gtest.h>

#include "PCMMerger.hpp"
#include "PCMUpdateLoggerWrapper.hpp"
#include "PredicatesCacheManager.hpp"
#include "PredicatesIndexFileBuilder.hpp"
#include "mock_structures/DataMergerMock.hpp"
#include "mock_structures/FHMock.hpp"
#include "mock_structures/StringIStream.hpp"

TEST(update_log_test, test_data_merger_mock) {
  K2TreeConfig config;
  config.treedepth = 32;
  config.cut_depth = 10;
  config.max_node_count = 256;
  DataMergerMock data_merger(config);
  std::string data;
  std::string data_offsets;
  std::string metadata;
  FHMock fh(data);
  FHMock fh_offsets(data_offsets);
  FHMock fh_metadata(metadata);
  UpdatesLogger updates_logger(data_merger, fh, fh_offsets, fh_metadata);

  int predicate_id_1 = 123;

  K2TreeMixed k2tree(config);
  K2TreeBulkOp op(k2tree);
  int size_tree = 10000;
  for (int i = 0; i < size_tree; i++) {
    op.insert(i + 1, i + 1);
  }
  K2TreeUpdates tree_update_1(predicate_id_1, &k2tree, nullptr);
  std::vector<K2TreeUpdates> tree_updates = {tree_update_1};
  updates_logger.log(tree_updates);

  ASSERT_TRUE(updates_logger.has_predicate_stored(predicate_id_1));
  ASSERT_EQ(k2tree.size(), size_tree);
  data_merger.drop();
  ASSERT_EQ(data_merger.trees.size(), 0);
  updates_logger.recover_all();
  ASSERT_EQ(data_merger.trees.size(), 1);

  auto &dm_tree = *data_merger.trees[predicate_id_1];

  ASSERT_EQ(dm_tree.size(), k2tree.size());
}

TEST(update_log_test, test_fhmock_can_sync_log_to_main_storage) {
  K2TreeConfig config;
  config.treedepth = 32;
  config.cut_depth = 10;
  config.max_node_count = 256;
  std::string data_updates;
  std::string data_offsets_updates;
  std::string metadata_updates;
  FHMock fh(data_updates);
  FHMock fh_offsets(data_offsets_updates);
  FHMock fh_metadata(metadata_updates);

  std::unique_ptr<I_FileRWHandler> fh_pcm{};
  {
    std::string cache_data;
    fh_pcm = std::make_unique<FHMock>(cache_data);
    auto fh_writer = fh_pcm->get_writer(std::ios::out | std::ios::binary);
    PredicatesCacheMetadata metadata_pcm(config);
    metadata_pcm.write_to_ostream(fh_writer->get_ostream());
    fh_writer->flush();
  }

  PredicatesCacheManager pcm(std::move(fh_pcm));
  PCMMerger merger(pcm);
  UpdatesLogger updates_logger(merger, fh, fh_offsets, fh_metadata);
  PCMUpdateLoggerWrapper pcm_upate_wrapper(updates_logger);
  pcm.set_update_logger(&pcm_upate_wrapper);

  int predicate_id_1 = 123;

  K2TreeMixed k2tree(config);
  K2TreeBulkOp op(k2tree);
  int size_tree = 10000;
  for (int i = 0; i < size_tree; i++) {
    op.insert(i + 1, i + 1);
  }
  K2TreeUpdates tree_update_1(predicate_id_1, &k2tree, nullptr);
  std::vector<K2TreeUpdates> tree_updates = {tree_update_1};
  updates_logger.log(tree_updates);

  ASSERT_TRUE(updates_logger.has_predicate_stored(predicate_id_1));
  ASSERT_EQ(k2tree.size(), size_tree);

  auto fetched = pcm.get_predicates_index_cache().fetch_k2tree(predicate_id_1);
  ASSERT_TRUE(fetched.exists());

  auto &fetched_k2tree = fetched.get_mutable();
  for (int i = 0; i < size_tree; i++) {
    ASSERT_TRUE(fetched_k2tree.has(i + 1, i + 1));
  }

  ASSERT_FALSE(
      pcm.get_predicates_index_cache().is_stored_in_main_index(predicate_id_1));

  pcm.get_predicates_index_cache().sync_to_persistent();

  ASSERT_TRUE(
      pcm.get_predicates_index_cache().is_stored_in_main_index(predicate_id_1));
}

TEST(update_log_test, can_compact_log_only_two_inserts) {
  K2TreeConfig config;
  config.treedepth = 32;
  config.cut_depth = 10;
  config.max_node_count = 256;
  std::string data_updates;
  std::string data_offsets_updates;
  std::string metadata_updates;
  FHMock fh(data_updates);
  FHMock fh_offsets(data_offsets_updates);
  FHMock fh_metadata(metadata_updates);

  std::unique_ptr<I_FileRWHandler> fh_pcm{};
  {
    std::string cache_data;
    fh_pcm = std::make_unique<FHMock>(cache_data);
    auto fh_writer = fh_pcm->get_writer(std::ios::out | std::ios::binary);
    PredicatesCacheMetadata metadata_pcm(config);
    metadata_pcm.write_to_ostream(fh_writer->get_ostream());
    fh_writer->flush();
  }

  PredicatesCacheManager pcm(std::move(fh_pcm));
  PCMMerger merger(pcm);
  UpdatesLogger updates_logger(merger, fh, fh_offsets, fh_metadata);
  PCMUpdateLoggerWrapper pcm_update_wrapper(updates_logger);
  pcm.set_update_logger(&pcm_update_wrapper);

  int predicate_id_1 = 123;

  K2TreeMixed k2tree(config);
  K2TreeBulkOp op(k2tree);
  int size_tree = 10000;
  for (int i = 0; i < size_tree; i++) {
    op.insert(i + 1, i + 1);
  }
  K2TreeMixed k2tree2(config);
  K2TreeBulkOp op2(k2tree2);
  for (int i = size_tree; i < 2 * size_tree; i++) {
    op2.insert(i + 1, i + 1);
  }
  K2TreeUpdates tree_update_1(predicate_id_1, &k2tree, nullptr);
  K2TreeUpdates tree_update_2(predicate_id_1, &k2tree2, nullptr);
  std::vector<K2TreeUpdates> tree_updates_1 = {tree_update_1};
  std::vector<K2TreeUpdates> tree_updates_2 = {tree_update_2};

  auto size_0 = fh.data.size();

  updates_logger.log(tree_updates_1);
  auto size_1 = fh.data.size();
  updates_logger.log(tree_updates_2);
  auto size_2 = fh.data.size();

  ASSERT_TRUE(updates_logger.has_predicate_stored(predicate_id_1));
  ASSERT_EQ(k2tree.size(), size_tree);

  auto fetched = pcm.get_predicates_index_cache().fetch_k2tree(predicate_id_1);
  ASSERT_TRUE(fetched.exists());

  auto &fetched_k2tree = fetched.get_mutable();
  for (int i = 0; i < 2 * size_tree; i++) {
    ASSERT_TRUE(fetched_k2tree.has(i + 1, i + 1));
  }

  ASSERT_FALSE(
      pcm.get_predicates_index_cache().is_stored_in_main_index(predicate_id_1));

  pcm.get_predicates_index_cache().sync_to_persistent();

  ASSERT_TRUE(
      pcm.get_predicates_index_cache().is_stored_in_main_index(predicate_id_1));

  ASSERT_EQ(updates_logger.logs_number(), 2);
  updates_logger.compact_logs();
  auto size_3 = fh.data.size();

  ASSERT_EQ(updates_logger.logs_number(), 1);

  ASSERT_EQ(size_0, 0);
  ASSERT_GT(size_1, size_0);
  ASSERT_GT(size_2, size_1);
  ASSERT_GT(size_2, size_3);
  ASSERT_GE(size_3, size_1);
}
TEST(update_log_test, can_compact_log_one_insert_one_delete) {
  K2TreeConfig config;
  config.treedepth = 32;
  config.cut_depth = 10;
  config.max_node_count = 256;
  std::string data_updates;
  std::string data_offsets_updates;
  std::string metadata_updates;
  FHMock fh(data_updates);
  FHMock fh_offsets(data_offsets_updates);
  FHMock fh_metadata(metadata_updates);

  std::unique_ptr<I_FileRWHandler> fh_pcm{};
  {
    std::string cache_data;
    fh_pcm = std::make_unique<FHMock>(cache_data);
    auto fh_writer = fh_pcm->get_writer(std::ios::out | std::ios::binary);
    PredicatesCacheMetadata metadata_pcm(config);
    metadata_pcm.write_to_ostream(fh_writer->get_ostream());
    fh_writer->flush();
  }

  PredicatesCacheManager pcm(std::move(fh_pcm));
  PCMMerger merger(pcm);
  UpdatesLogger updates_logger(merger, fh, fh_offsets, fh_metadata);
  PCMUpdateLoggerWrapper pcm_update_wrapper(updates_logger);
  pcm.set_update_logger(&pcm_update_wrapper);

  int predicate_id_1 = 123;

  K2TreeMixed k2tree(config);
  K2TreeBulkOp op(k2tree);
  int size_tree = 10000;
  for (int i = 0; i < size_tree; i++) {
    op.insert(i + 1, i + 1);
  }
  K2TreeMixed k2tree2(config);
  K2TreeBulkOp op2(k2tree2);
  for (int i = 0; i < size_tree; i++) {
    op2.insert(i + 1, i + 1);
  }
  K2TreeUpdates tree_update_1(predicate_id_1, &k2tree, nullptr);
  K2TreeUpdates tree_update_2(predicate_id_1, nullptr, &k2tree2);
  std::vector<K2TreeUpdates> tree_updates_1 = {tree_update_1};
  std::vector<K2TreeUpdates> tree_updates_2 = {tree_update_2};

  auto size_0 = fh.data.size();

  updates_logger.log(tree_updates_1);
  auto size_1 = fh.data.size();
  updates_logger.log(tree_updates_2);
  auto size_2 = fh.data.size();

  ASSERT_TRUE(updates_logger.has_predicate_stored(predicate_id_1));
  ASSERT_EQ(k2tree.size(), size_tree);

  auto fetched = pcm.get_predicates_index_cache().fetch_k2tree(predicate_id_1);
  ASSERT_TRUE(fetched.exists());

  ASSERT_FALSE(
      pcm.get_predicates_index_cache().is_stored_in_main_index(predicate_id_1));

  pcm.get_predicates_index_cache().sync_to_persistent();

  ASSERT_TRUE(
      pcm.get_predicates_index_cache().is_stored_in_main_index(predicate_id_1));

  ASSERT_EQ(updates_logger.logs_number(), 2);
  updates_logger.compact_logs();
  auto size_3 = fh.data.size();

  ASSERT_EQ(updates_logger.logs_number(), 1);

  ASSERT_EQ(size_0, 0);
  ASSERT_GT(size_1, size_0);
  ASSERT_GT(size_2, size_1);
  ASSERT_GT(size_2, size_3);
  ASSERT_GE(size_3, size_0);
  ASSERT_GT(size_1, size_3);

  auto fetched_after_compaction =
      pcm.get_predicates_index_cache().fetch_k2tree(predicate_id_1);
  ASSERT_TRUE(fetched_after_compaction.exists());
  ASSERT_EQ(fetched_after_compaction.get_mutable().size(), 0);

  auto after_compaction_scanner =
      fetched_after_compaction.get_mutable().create_full_scanner();
  ASSERT_FALSE(after_compaction_scanner->has_next());



}
