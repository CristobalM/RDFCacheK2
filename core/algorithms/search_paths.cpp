//
// Created by cristobal on 11-12-22.
//

#include <random>
#include <unordered_set>
#include <vector>

#include "search_paths.hpp"
#include "DirectedPath.hpp"


namespace k2cache {

unsigned long choose_predicate_index(unsigned long size);
DirectedPath find_new_npath(PredicatesCacheManager &pcm, int n,
                            bool discard_intermediate,
                            const std::unordered_set<DirectedPath> &paths);

std::vector<DirectedPath> find_n_paths(PredicatesCacheManager &pcm, int n,
                                       int max_number,
                                       bool discard_intermediate) {

  const auto &predicate_ids =
      pcm.get_predicates_index_cache().get_predicates_ids();

  std::unordered_set<DirectedPath> paths;

  for(int cnt = 0; cnt < max_number; cnt++){
    auto path = find_new_npath(pcm, n, discard_intermediate, paths);
  }


  return {paths.begin(), paths.end()};
}


DirectedPath find_new_npath(PredicatesCacheManager &pcm, int n,
                            bool discard_intermediate,
                            const std::unordered_set<DirectedPath> &paths) {
  DirectedPath result;

  const auto &predicate_ids = pcm.get_predicates_index_cache().get_predicates_ids();
  auto pidx = choose_predicate_index(predicate_ids.size());
  auto pred = predicate_ids[pidx];

  return result;
}


unsigned long choose_predicate_index(unsigned long size) {
  if(size == 0){
    return 0;
  }
  static std::random_device rd;
  static std::mt19937 gen(rd());
  static std::uniform_int_distribution<unsigned long> dis(0, size-1UL);
  return dis(gen);
}

} // namespace k2cache