//
// Created by cristobal on 14-12-22.
//

#include <gtest/gtest.h>

#include "cache_test_util.hpp"
#include <algorithms/search_paths.hpp>

using namespace k2cache;

TEST(search_paths_test, can_search_simple_paths_1){

  std::vector<TripleValue> triples = {
      {1, 2, 3},
      {3, 4, 5},
  };

  std::vector<unsigned long> node_ids = {1, 2, 3, 4, 5};

  K2TreeConfig config;
  config.max_node_count = 128;
  config.treedepth = 32;
  config.cut_depth = 0;

  auto cc = mock_cache_container(triples, node_ids, config, false);

  auto &pcm = cc->cache_container->get_pcm();
  ASSERT_NE(&pcm ,nullptr);
  pcm.load_all_predicates();
  auto paths = find_n_paths(pcm, 2, 1);

  ASSERT_EQ(paths.size(), 1);
  ASSERT_EQ(paths[0].first, 1);
  ASSERT_EQ(paths[0].second, 5);

}