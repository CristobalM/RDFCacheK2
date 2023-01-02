//
// Created by cristobal on 04-08-21.
//

#ifndef RDFCACHEK2_LRUREPLACEMENTSTRATEGY_HPP
#define RDFCACHEK2_LRUREPLACEMENTSTRATEGY_HPP

#include "I_ReplacementPriorityQueue.hpp"
#include <map>
#include <unordered_map>
namespace k2cache {
class LRUReplacementStrategy {
  long low;
  long high;
  std::map<uint64_t, long> key_mapping_to_lru_value;

public:
  LRUReplacementStrategy();

  LRUReplacementStrategy(LRUReplacementStrategy &other) = delete;
  LRUReplacementStrategy &operator=(LRUReplacementStrategy &other) = delete;

  long cost_function(uint64_t key) const;

  void hit_key(uint64_t key);
  void remove_key(uint64_t key);
  bool should_discard_others_for_key(uint64_t key,
                                     I_ReplacementPriorityQueue &pq);
  bool should_hit_without_retrieval(uint64_t key,
                                    I_ReplacementPriorityQueue &pq);
};
} // namespace k2cache

#endif // RDFCACHEK2_LRUREPLACEMENTSTRATEGY_HPP
