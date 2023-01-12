//
// Created by cristobal on 14-12-22.
//

#include <gtest/gtest.h>

#include "cache_test_util.hpp"
#include <algorithms/search_paths.hpp>

using namespace k2cache;

std::vector<uint64_t> get_consecutive_seq(int size) {
  std::vector<uint64_t> sequence;
  sequence.reserve(size);
  for (int i = 1; i <= size; i++) {
    sequence.push_back(i);
  }
  return sequence;
}



// TODO: refactor to only use RDFTriple eveywhere
static std::vector<TripleValue> triple_value_vec_from_rdf_triple_vec(
    const std::vector<RDFTriple> &v
){
  std::vector<TripleValue> result;
  result.reserve(v.size());
  for(const auto &t: v){
    result.emplace_back(t.subject, t.predicate, t.object);
  }
  return result;
}

static std::vector<DirectedPath>
find_n_paths_for_input(const std::vector<RDFTriple> &triples,
                       const std::vector<uint64_t> &node_ids,
                       int n,
                       int max_number
                       ) {
  K2TreeConfig config{};
  config.max_node_count = 128;
  config.treedepth = 32;
  config.cut_depth = 0;

  auto triple_value = triple_value_vec_from_rdf_triple_vec(triples);
  auto cc = mock_cache_container(triple_value, node_ids, config, false);
  auto &pcm = cc->cache_container->get_pcm();
  pcm.load_all_predicates();
  auto paths = find_n_paths(pcm, n, max_number);

  return paths;
}

static bool same_path(const DirectedPath &lhs, const DirectedPath &rhs) {
  return lhs == rhs;
}

TEST(search_paths_test, can_search_simple_paths_1) {

  std::vector<RDFTriple> triples = {
      {1, 2, 3},
      {3, 4, 5},
  };

  std::vector<uint64_t> node_ids = get_consecutive_seq(5);

  auto paths = find_n_paths_for_input(triples, node_ids, 2, 1);

  ASSERT_EQ(paths.size(), 1);
  ASSERT_EQ(paths[0].get_vec()[0].subject, 1);
  ASSERT_EQ(paths[0].get_vec()[1].object, 5);
  ASSERT_TRUE(same_path(paths[0], DirectedPath(triples)));
}

TEST(search_paths_test, can_search_simple_paths_2) {
  std::vector<RDFTriple> triples = {
      {1, 2, 3},
      {3, 4, 5},
      {3, 6, 7},
  };
  std::vector<uint64_t> node_ids = get_consecutive_seq(7);

  auto paths = find_n_paths_for_input(triples, node_ids, 2, 2);

  ASSERT_EQ(paths.size(), 2);
  std::sort(paths.begin(), paths.end());
  ASSERT_TRUE(same_path(paths[0], DirectedPath(
                                      {
                                          {1, 2, 3},
                                          {3, 4, 5},
                                      }
                                      )));
  ASSERT_TRUE(same_path(paths[1], DirectedPath(
                                      {
                                          {1, 2, 3},
                                          {3, 6, 7},
                                      }
                                      )));
//  ASSERT_EQ(paths[0] == DirectedPath({}{}))
//  ASSERT_TRUE(same_pair(paths[0], {1, 5}));
//  ASSERT_TRUE(same_pair(paths[1], {1, 7}));
}

TEST(search_paths_test, can_search_simple_paths_3) {
  std::vector<RDFTriple> triples = {
      {1, 2, 3},
      {3, 4, 5},
      {3, 6, 7},
      {7, 1, 2},
  };
  std::vector<uint64_t> node_ids = get_consecutive_seq(7);

  auto paths = find_n_paths_for_input(triples, node_ids, 2, 3);

  ASSERT_EQ(paths.size(), 3);
  std::sort(paths.begin(), paths.end());
  ASSERT_TRUE(same_path(paths[0], DirectedPath({
                                      {1, 2, 3},
                                      {3, 4, 5}
                                  })));
  ASSERT_TRUE(same_path(paths[1], DirectedPath({
                                      {1, 2, 3},
                                      {3, 6, 7}
                                  })));
  ASSERT_TRUE(same_path(paths[2], DirectedPath({
                                      {3, 6, 7},
                                      {7, 1, 2}
                                  })));
}

TEST(search_paths_test, can_search_simple_paths_3_max_number_works) {
  std::vector<RDFTriple> triples = {
      {1, 2, 3},
      {3, 4, 5},
      {3, 6, 7},
      {7, 1, 2},
  };
  std::vector<uint64_t> node_ids = get_consecutive_seq(7);

  auto paths = find_n_paths_for_input(triples, node_ids, 2, 1);

  ASSERT_EQ(paths.size(), 1);
  std::sort(paths.begin(), paths.end());

  const auto possible_paths = std::vector<DirectedPath>{
      DirectedPath({{1, 2, 3}, {3,4, 5}}),
      DirectedPath({{1, 2, 3}, {3,6, 7}}),
      DirectedPath({{3, 6, 7}, {7,1, 2}}),
  };
  bool found = false;
  for(const auto &p: possible_paths){
    if(same_path(paths[0], p)){
      found = true;
      break;
    }
  }
  ASSERT_TRUE(found);
}

TEST(search_paths_test, can_find_simple_path_100) {
  std::vector<RDFTriple> triples;

  const auto sz = 100UL;

  for(auto i = 1UL; i <= sz; i+=2){
    triples.emplace_back(i, i+1, i+2);
  }

  std::vector<uint64_t> node_ids = get_consecutive_seq(sz);

  auto paths = find_n_paths_for_input(triples, node_ids, 50, 1);

  ASSERT_EQ(paths.size(), 1);
  std::sort(paths.begin(), paths.end());
  ASSERT_TRUE(same_path(paths[0], DirectedPath(triples)));
}
