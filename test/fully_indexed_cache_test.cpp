//
// Created by cristobal on 11-06-22.
//
#include "I_FileRWHandler.hpp"
#include "K2TreeMixed.hpp"
#include "PredicatesCacheManager.hpp"
#include "PredicatesCacheMetadata.hpp"
#include "cache_test_util.hpp"
#include "mock_structures/FHMock.hpp"
#include <gtest/gtest.h>

TEST(fully_indexed_cache, test_resynced_on_update){
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
  std::vector<unsigned long> predicates = {predicate_id_1, predicate_id_2};
  pcm.get_fully_indexed_cache().init_streamer_predicates(predicates);

  auto fi_resp_1 = pcm.get_fully_indexed_cache().get(predicate_id_1);
  ASSERT_TRUE(fi_resp_1.exists());
  for (int i = 0; i < size_tree; i++) {
    ASSERT_TRUE(fi_resp_1.get()->has(i+1, i+1));
  }
  
}