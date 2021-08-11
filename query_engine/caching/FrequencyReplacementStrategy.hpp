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

  bool operator()(unsigned long lhs, unsigned long rhs) const;

  void hit_key(unsigned long key);
  void remove_key(unsigned long key);
  long get_frequency(unsigned long key) const;
};

#endif // RDFCACHEK2_FREQUENCYREPLACEMENTSTRATEGY_HPP
