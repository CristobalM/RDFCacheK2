//
// Created by cristobal on 11-06-22.
//
#include "I_FileRWHandler.hpp"
#include "cache_test_util.hpp"
#include "fic/CacheDataManager.hpp"
#include "k2tree/K2TreeBulkOp.hpp"
#include "k2tree/K2TreeMixed.hpp"
#include "manager/PCMFactory.hpp"
#include "manager/PredicatesCacheMetadata.hpp"
#include "mock_structures/FHMock.hpp"
#include "mock_structures/MockFetcher.hpp"
#include "replacement/NoCachingReplacement.hpp"
#include <gtest/gtest.h>

using namespace k2cache;

TEST(fully_indexed_cache, test_resynced_on_update_unloaded) {
  K2TreeConfig config{};
  config.treedepth = 32;
  config.cut_depth = 10;
  config.max_node_count = 256;

  std::unique_ptr<I_FileRWHandler> fh_pcm{};
  DataHolders h1;
  {
    fh_pcm = std::make_unique<FHMock>(h1.pcm_h.data);
    auto fh_writer = fh_pcm->get_writer(std::ios::out | std::ios::binary);
    PredicatesCacheMetadata metadata_pcm(config);
    metadata_pcm.write_to_ostream(fh_writer->get_ostream());
    fh_writer->flush();
  }
  DataHolders h2;
  auto pcm =
      PCMFactory::create(std::move(fh_pcm), mock_fh_manager(h2.pcm_h), true);
  auto &updates_logger = pcm->get_updates_logger();

  unsigned long predicate_id_1 = 123;
  unsigned long predicate_id_2 = 333222;

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

  // fully indexed needs that predicates are in memory
  // and logging them doesn't do that automatically
  pcm->load_all_predicates();

  std::vector<unsigned long> predicates = {predicate_id_1, predicate_id_2};
  pcm->get_fully_indexed_cache().init_streamer_predicates(predicates);

  auto fi_resp_1 = pcm->get_fully_indexed_cache().get(predicate_id_1);
  ASSERT_TRUE(fi_resp_1.exists());
  for (int i = 0; i < size_tree; i++) {
    ASSERT_TRUE(fi_resp_1.get()->has(i + 1, i + 1));
  }
}
TEST(fully_indexed_cache, test_resynced_on_update_loaded) {
  K2TreeConfig config{};
  config.treedepth = 32;
  config.cut_depth = 10;
  config.max_node_count = 256;

  std::unique_ptr<I_FileRWHandler> fh_pcm{};
  DataHolders h1;
  {
    fh_pcm = std::make_unique<FHMock>(h1.pcm_h.data);
    auto fh_writer = fh_pcm->get_writer(std::ios::out | std::ios::binary);
    PredicatesCacheMetadata metadata_pcm(config);
    metadata_pcm.write_to_ostream(fh_writer->get_ostream());
    fh_writer->flush();
  }
  DataHolders h2;
  auto pcm =
      PCMFactory::create(std::move(fh_pcm), mock_fh_manager(h2.pcm_h), true);
  auto &updates_logger = pcm->get_updates_logger();

  unsigned long predicate_id_1 = 123;
  unsigned long predicate_id_2 = 333222;

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

  // fully indexed needs that predicates are in memory
  // and logging them doesn't do that automatically
  pcm->load_all_predicates();

  std::vector<unsigned long> predicates = {predicate_id_1, predicate_id_2};
  pcm->get_fully_indexed_cache().init_streamer_predicates(predicates);

  auto fi_resp_1 = pcm->get_fully_indexed_cache().get(predicate_id_1);
  ASSERT_TRUE(fi_resp_1.exists());
  for (int i = 0; i < size_tree; i++) {
    ASSERT_TRUE(fi_resp_1.get()->has(i + 1, i + 1));
  }

  K2TreeMixed k2tree3(config);
  K2TreeBulkOp op3(k2tree);
  for (int i = 0; i < size_tree; i++) {
    op3.insert(size_tree + i + 1, size_tree + i + 1);
  }
  K2TreeUpdates tree_update_3(predicate_id_2, &k2tree3, nullptr);
  std::vector<K2TreeUpdates> tree_updates_2 = {tree_update_3};

  updates_logger.log(tree_updates_2);

  auto fi_resp_2 = pcm->get_fully_indexed_cache().get(predicate_id_2);
  for (int i = 0; i < size_tree; i++) {
    ASSERT_TRUE(fi_resp_2.get()->has(size_tree + i + 1, size_tree + i + 1));
  }
  ASSERT_FALSE(fi_resp_2.get()->has(2 * size_tree + 1, 2 * size_tree + 1));
}

TEST(fully_indexed_cache, test_impl_1) {
  auto fetcher = std::make_unique<MockFetcher>();
  auto map = std::make_unique<fic::types::cache_map_t>();
  auto dm = std::make_unique<CacheDataManager>(*map, *fetcher);
  auto cr = std::make_unique<NoCachingReplacement>();
  FullyIndexedCacheImpl idx(*fetcher, std::move(map), std::move(dm),
                            std::move(cr));

  std::vector<unsigned long> predicates = {
      1, 2, 3, 4, 5,
  };

  idx.init_streamer_predicates(predicates);
  
  for(auto p: predicates){
    auto sc = idx.should_cache(p);
    ASSERT_TRUE(sc == 1);
  }

  for(auto i = 6; i < 100; i++){
    auto sc = idx.should_cache(i);
    ASSERT_FALSE(sc == 1);
  }
}
