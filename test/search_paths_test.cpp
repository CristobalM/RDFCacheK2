//
// Created by cristobal on 14-12-22.
//

#include <gtest/gtest.h>

#include "cache_test_util.hpp"
#include <algorithms/search_paths.hpp>

using namespace k2cache;

std::vector<unsigned long> get_consecutive_seq(int size) {
  std::vector<unsigned long> sequence;
  sequence.reserve(size);
  for (int i = 1; i <= size; i++) {
    sequence.push_back(i);
  }
  return sequence;
}

using pul_t = std::pair<unsigned long, unsigned long>;

static std::vector<pul_t>
find_n_paths_for_input(const std::vector<TripleValue> &triples,
                       const std::vector<unsigned long> &node_ids,
                       int n,
                       int max_number
                       ) {
  K2TreeConfig config;
  config.max_node_count = 128;
  config.treedepth = 32;
  config.cut_depth = 0;

  auto cc = mock_cache_container(triples, node_ids, config, false);
  auto &pcm = cc->cache_container->get_pcm();
  pcm.load_all_predicates();
  auto paths = find_n_paths(pcm, n, max_number);

  return paths;
}

static bool same_pair(const pul_t &lhs, const pul_t &rhs) { return lhs == rhs; }

TEST(search_paths_test, can_search_simple_paths_1) {

  std::vector<TripleValue> triples = {
      {1, 2, 3},
      {3, 4, 5},
  };
  std::vector<unsigned long> node_ids = get_consecutive_seq(5);

  auto paths = find_n_paths_for_input(triples, node_ids, 2, 1);

  ASSERT_EQ(paths.size(), 1);
  ASSERT_EQ(paths[0].first, 1);
  ASSERT_EQ(paths[0].second, 5);
  ASSERT_TRUE(same_pair(paths[0], {1, 5}));
}

TEST(search_paths_test, can_search_simple_paths_2) {
  std::vector<TripleValue> triples = {
      {1, 2, 3},
      {3, 4, 5},
      {3, 6, 7},
  };
  std::vector<unsigned long> node_ids = get_consecutive_seq(7);

  auto paths = find_n_paths_for_input(triples, node_ids, 2, 2);

  ASSERT_EQ(paths.size(), 2);
  std::sort(paths.begin(), paths.end(), [](const pul_t &lhs, const pul_t &rhs) {
    return (lhs.first == rhs.first && lhs.second < rhs.second) ||
           lhs.first < rhs.first;
  });
  ASSERT_TRUE(same_pair(paths[0], {1, 5}));
  ASSERT_TRUE(same_pair(paths[1], {1, 7}));
}

TEST(search_paths_test, can_search_simple_paths_3) {
  std::vector<TripleValue> triples = {
      {1, 2, 3},
      {3, 4, 5},
      {3, 6, 7},
      {7, 1, 2},
  };
  std::vector<unsigned long> node_ids = get_consecutive_seq(7);

  auto paths = find_n_paths_for_input(triples, node_ids, 2, 3);

  ASSERT_EQ(paths.size(), 3);
  std::sort(paths.begin(), paths.end(), [](const pul_t &lhs, const pul_t &rhs) {
    return (lhs.first == rhs.first && lhs.second < rhs.second) ||
           lhs.first < rhs.first;
  });
  ASSERT_TRUE(same_pair(paths[0], {1, 5}));
  ASSERT_TRUE(same_pair(paths[1], {1, 7}));
  ASSERT_TRUE(same_pair(paths[2], {3, 2}));
}

TEST(search_paths_test, can_search_simple_paths_3_max_number_works) {
  std::vector<TripleValue> triples = {
      {1, 2, 3},
      {3, 4, 5},
      {3, 6, 7},
      {7, 1, 2},
  };
  std::vector<unsigned long> node_ids = get_consecutive_seq(7);

  auto paths = find_n_paths_for_input(triples, node_ids, 2, 1);

  ASSERT_EQ(paths.size(), 1);
  std::sort(paths.begin(), paths.end(), [](const pul_t &lhs, const pul_t &rhs) {
    return (lhs.first == rhs.first && lhs.second < rhs.second) ||
           lhs.first < rhs.first;
  });
}

TEST(search_paths_test, can_find_simple_path_100) {
  std::vector<TripleValue> triples;

  const auto sz = 100UL;

  for(auto i = 1UL; i <= sz; i+=2){
    triples.emplace_back(i, i+1, i+2);
  }

  std::vector<unsigned long> node_ids = get_consecutive_seq(sz);

  auto paths = find_n_paths_for_input(triples, node_ids, 50, 1);

  ASSERT_EQ(paths.size(), 1);
  std::sort(paths.begin(), paths.end(), [](const pul_t &lhs, const pul_t &rhs) {
    return (lhs.first == rhs.first && lhs.second < rhs.second) ||
           lhs.first < rhs.first;
  });
  ASSERT_TRUE(same_pair(paths[0], {1, 101}));
}
