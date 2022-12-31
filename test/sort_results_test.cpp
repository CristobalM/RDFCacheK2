//
// Created by cristobal on 07-11-22.
//

#include "cache_test_util.hpp"
#include "streaming/TPMSortedStreamer.hpp"
#include <gtest/gtest.h>

using namespace k2cache;

TEST(sort_results_test, can_get_correct_results){
  auto nids = std::vector<uint64_t>{3, 3231, 9222, 123111, 828282828};
  auto triples = std::vector<TripleValue>{
      {1, 2, 3},
      {1, 3, 2},
      {2, 3, 4},
  };
  K2TreeConfig config;
  config.cut_depth = 0;
  config.max_node_count = 128;
  config.treedepth = 32;
  auto td_wrapper = mock_cache_container(triples, nids, config, true);
  td_wrapper->cache_container->get_pcm().load_all_predicates();
//  TripleNodeId triple_node_id(
//      NodeId()
//  );
//  TPMSortedStreamer streamer(0, 0, )
}