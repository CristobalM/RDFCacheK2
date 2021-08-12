//
// Created by cristobal on 11-08-21.
//

#ifndef RDFCACHEK2_FREQUENCYREPLACEMENTSTRATEGY_HPP
#define RDFCACHEK2_FREQUENCYREPLACEMENTSTRATEGY_HPP

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
};

#endif // RDFCACHEK2_FREQUENCYREPLACEMENTSTRATEGY_HPP
