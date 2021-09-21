//
// Created by cristobal on 11-08-21.
//

#ifndef RDFCACHEK2_FREQUENCYREPLACEMENTSTRATEGY_HPP
#define RDFCACHEK2_FREQUENCYREPLACEMENTSTRATEGY_HPP

#include "I_ReplacementPriorityQueue.hpp"
#include <map>

class FrequencyReplacementStrategy {

  std::map<unsigned long, long> frequency_map;

public:
  FrequencyReplacementStrategy();

  FrequencyReplacementStrategy(FrequencyReplacementStrategy &other) = delete;
  FrequencyReplacementStrategy &
  operator=(FrequencyReplacementStrategy &other) = delete;

  long cost_function(unsigned long key) const;

  void hit_key(unsigned long key);
  void remove_key(unsigned long key);
  bool should_discard_others_for_key(unsigned long key,
                                     I_ReplacementPriorityQueue &pq);
  bool should_hit_without_retrieval(unsigned long key,
                                    I_ReplacementPriorityQueue &pq);
};

#endif // RDFCACHEK2_FREQUENCYREPLACEMENTSTRATEGY_HPP
