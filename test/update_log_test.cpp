//
// Created by cristobal on 9/6/21.
//

#include "UpdatesLogger.hpp"
#include <K2TreeBulkOp.hpp>
#include <gtest/gtest.h>

#include "PredicatesCacheManager.hpp"
#include "PredicatesIndexFileBuilder.hpp"
#include "cache_test_util.hpp"
#include "mock_structures/DataMergerMock.hpp"
#include "mock_structures/FHMock.hpp"
#include "mock_structures/StringIStream.hpp"

TEST(update_log_test, test_data_merger_mock) {
  K2TreeConfig config;
  config.treedepth = 32;
  config.cut_depth = 10;
  config.max_node_count = 256;
  DataMergerMock data_merger(config);
  UpdatesLogger updates_logger(data_merger, mock_fh_manager());

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

  PredicatesCacheManager pcm(std::move(fh_pcm), mock_fh_manager());

  int predicate_id_1 = 123;

  K2TreeMixed k2tree(config);
  K2TreeBulkOp op(k2tree);
  int size_tree = 10000;
  for (int i = 0; i < size_tree; i++) {
    op.insert(i + 1, i + 1);
  }
  K2TreeUpdates tree_update_1(predicate_id_1, &k2tree, nullptr);
  std::vector<K2TreeUpdates> tree_updates = {tree_update_1};
  pcm.get_updates_logger().log(tree_updates);

  ASSERT_TRUE(pcm.get_updates_logger().has_predicate_stored(predicate_id_1));
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


static unsigned long read_stream_size(I_FileRWHandler &fh){
  auto fh_reader = fh.get_reader(std::ios::binary);
  auto &is = fh_reader->get_istream();
  is.seekg(0, std::istream::end);
  return is.tellg();
}

TEST(update_log_test, test_can_compact_log_only_two_inserts) {
  K2TreeConfig config;
  config.treedepth = 32;
  config.cut_depth = 10;
  config.max_node_count = 256;
  std::unique_ptr<I_FileRWHandler> fh_pcm{};
  {
    std::string cache_data;
    fh_pcm = std::make_unique<FHMock>(cache_data);
    auto fh_writer = fh_pcm->get_writer(std::ios::out | std::ios::binary);
    PredicatesCacheMetadata metadata_pcm(config);
    metadata_pcm.write_to_ostream(fh_writer->get_ostream());
    fh_writer->flush();
  }

  PredicatesCacheManager pcm(std::move(fh_pcm), mock_fh_manager());

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

  auto &fh = pcm.get_updates_logger().get_fh_manager().get_index_logs_fh();

  auto size_0 = read_stream_size(fh);

  auto &updates_logger = pcm.get_updates_logger();

  updates_logger.log(tree_updates_1);
  auto size_1 = read_stream_size(fh);
  updates_logger.log(tree_updates_2);
  auto size_2 = read_stream_size(fh);

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
  auto size_3 =  read_stream_size(fh);

  ASSERT_EQ(updates_logger.logs_number(), 1);

  ASSERT_EQ(size_0, 0);
  ASSERT_GT(size_1, size_0);
  ASSERT_GT(size_2, size_1);
  ASSERT_GT(size_2, size_3);
  ASSERT_GE(size_3, size_1);

  // this should be idempotent
  updates_logger.compact_logs();
  ASSERT_EQ(updates_logger.logs_number(), 1);
  auto size_last = read_stream_size(fh);
  ASSERT_EQ(size_last, size_3);
}

TEST(update_log_test, test_can_compact_log_one_insert_one_delete) {
  K2TreeConfig config;
  config.treedepth = 32;
  config.cut_depth = 10;
  config.max_node_count = 256;

  std::unique_ptr<I_FileRWHandler> fh_pcm{};
  {
    std::string cache_data;
    fh_pcm = std::make_unique<FHMock>(cache_data);
    auto fh_writer = fh_pcm->get_writer(std::ios::out | std::ios::binary);
    PredicatesCacheMetadata metadata_pcm(config);
    metadata_pcm.write_to_ostream(fh_writer->get_ostream());
    fh_writer->flush();
  }

  PredicatesCacheManager pcm(std::move(fh_pcm), mock_fh_manager());
  auto &updates_logger = pcm.get_updates_logger();

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
  auto &fh = pcm.get_updates_logger().get_fh_manager().get_index_logs_fh();


  auto size_0 =  read_stream_size(fh);

  updates_logger.log(tree_updates_1);
  auto size_1 =  read_stream_size(fh);
  updates_logger.log(tree_updates_2);
  auto size_2 =  read_stream_size(fh);

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
  auto size_3 =  read_stream_size(fh);

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

TEST(update_log_test, test_update_unloaded_predicates_from_logs) {
  K2TreeConfig config;
  config.treedepth = 32;
  config.cut_depth = 10;
  config.max_node_count = 256;

  std::unique_ptr<I_FileRWHandler> fh_pcm{};
  {
    std::string cache_data;
    fh_pcm = std::make_unique<FHMock>(cache_data);
    auto fh_writer = fh_pcm->get_writer(std::ios::out | std::ios::binary);
    PredicatesCacheMetadata metadata_pcm(config);
    metadata_pcm.write_to_ostream(fh_writer->get_ostream());
    fh_writer->flush();
  }
  PredicatesCacheManager pcm(std::move(fh_pcm), mock_fh_manager());
  auto &updates_logger = pcm.get_updates_logger();

  int predicate_id_1 = 123;
  int predicate_id_2 = 333222;

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
  K2TreeUpdates tree_update_2(predicate_id_2, &k2tree2, nullptr);
  std::vector<K2TreeUpdates> tree_updates_1 = {tree_update_1, tree_update_2};

  updates_logger.log(tree_updates_1);
  ASSERT_EQ(updates_logger.logs_number(), 1);

  ASSERT_TRUE(updates_logger.has_predicate_stored(predicate_id_1));
  ASSERT_TRUE(updates_logger.has_predicate_stored(predicate_id_2));
  ASSERT_EQ(k2tree.size(), size_tree);
  ASSERT_EQ(k2tree2.size(), size_tree);

  auto fetched = pcm.get_predicates_index_cache().fetch_k2tree(predicate_id_1);
  ASSERT_TRUE(fetched.exists());

  auto fetched2 = pcm.get_predicates_index_cache().fetch_k2tree(predicate_id_2);
  ASSERT_TRUE(fetched2.exists());

  auto &fetched_k2tree = fetched.get_mutable();
  for (int i = 0; i < size_tree; i++) {
    ASSERT_TRUE(fetched_k2tree.has(i + 1, i + 1));
  }
  auto &fetched_k2tree2 = fetched2.get_mutable();
  for (int i = size_tree; i < 2 * size_tree; i++) {
    ASSERT_TRUE(fetched_k2tree2.has(i + 1, i + 1));
  }

  ASSERT_FALSE(
      pcm.get_predicates_index_cache().is_stored_in_main_index(predicate_id_1));
  ASSERT_FALSE(
      pcm.get_predicates_index_cache().is_stored_in_main_index(predicate_id_2));

  pcm.get_predicates_index_cache().sync_to_persistent();

  ASSERT_TRUE(
      pcm.get_predicates_index_cache().is_stored_in_main_index(predicate_id_1));
  ASSERT_TRUE(
      pcm.get_predicates_index_cache().is_stored_in_main_index(predicate_id_2));

  K2TreeMixed k2tree3(config);
  K2TreeBulkOp op3(k2tree3);
  for (int i = 2 * size_tree; i < 3 * size_tree; i++) {
    op3.insert(i + 1, i + 1);
  }

  ASSERT_TRUE(
      pcm.get_predicates_index_cache().has_predicate_active(predicate_id_2));
  pcm.get_predicates_index_cache().discard_in_memory_predicate(predicate_id_2);
  ASSERT_FALSE(
      pcm.get_predicates_index_cache().has_predicate_active(predicate_id_2));

  K2TreeUpdates tree_update_3(predicate_id_2, &k2tree3, nullptr);
  std::vector<K2TreeUpdates> updates = {tree_update_3};

  ASSERT_EQ(updates_logger.logs_number(), 1);
  updates_logger.log(updates);
  ASSERT_EQ(updates_logger.logs_number(), 2);

  pcm.get_predicates_index_cache().full_sync_logs_and_memory_with_persistent();
  ASSERT_EQ(updates_logger.logs_number(), 0);

  ASSERT_FALSE(
      pcm.get_predicates_index_cache().has_predicate_active(predicate_id_2));

  auto fetched_2_after_sync =
      pcm.get_predicates_index_cache().fetch_k2tree(predicate_id_2);
  ASSERT_TRUE(fetched_2_after_sync.exists());

  auto k2_after_sync = fetched_2_after_sync.get_mutable();

  for (int i = size_tree; i < 3 * size_tree; i++) {
    ASSERT_TRUE(k2_after_sync.has(i + 1, i + 1));
  }

  ASSERT_TRUE(
      pcm.get_predicates_index_cache().has_predicate_active(predicate_id_1));
  pcm.get_predicates_index_cache().discard_in_memory_predicate(predicate_id_1);
  ASSERT_FALSE(
      pcm.get_predicates_index_cache().has_predicate_active(predicate_id_1));
  auto fetched_1_after_sync =
      pcm.get_predicates_index_cache().fetch_k2tree(predicate_id_1);
  ASSERT_TRUE(
      pcm.get_predicates_index_cache().has_predicate_active(predicate_id_1));

  ASSERT_TRUE(fetched_1_after_sync.exists());
  auto &k2_1 = fetched_1_after_sync.get_mutable();
  for (int i = 0; i < size_tree; i++) {
    ASSERT_TRUE(k2_1.has(i + 1, i + 1));
  }
}
