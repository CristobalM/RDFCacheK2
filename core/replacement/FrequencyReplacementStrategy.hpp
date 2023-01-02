//
// Created by cristobal on 11-08-21.
//

#ifndef RDFCACHEK2_FREQUENCYREPLACEMENTSTRATEGY_HPP
#define RDFCACHEK2_FREQUENCYREPLACEMENTSTRATEGY_HPP

#include "I_ReplacementPriorityQueue.hpp"
#include <map>

namespace k2cache {
class FrequencyReplacementStrategy {

  std::map<uint64_t, long> frequency_map;

public:
  FrequencyReplacementStrategy();

  FrequencyReplacementStrategy(FrequencyReplacementStrategy &other) = delete;
  FrequencyReplacementStrategy &
  operator=(FrequencyReplacementStrategy &other) = delete;

  long cost_function(uint64_t key) const;

  void hit_key(uint64_t key);
  void remove_key(uint64_t key);
  bool should_discard_others_for_key(uint64_t key,
                                     I_ReplacementPriorityQueue &pq);
  bool should_hit_without_retrieval(uint64_t key,
                                    I_ReplacementPriorityQueue &pq);
};
} // namespace k2cache

#endif // RDFCACHEK2_FREQUENCYREPLACEMENTSTRATEGY_HPP
