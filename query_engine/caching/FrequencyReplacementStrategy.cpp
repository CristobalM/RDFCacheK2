//
// Created by cristobal on 11-08-21.
//

#include "FrequencyReplacementStrategy.hpp"
FrequencyReplacementStrategy::FrequencyReplacementStrategy() = default;

void FrequencyReplacementStrategy::hit_key(unsigned long key) {
  auto it = frequency_map.find(key);
  if (it == frequency_map.end()) {
    frequency_map[key] = 1;
    return;
  }
  it->second++;
}

// we do nothing here, as hit frequency count shouldn't be affected
// when keys are discarded
void FrequencyReplacementStrategy::remove_key(unsigned long) {}
long FrequencyReplacementStrategy::cost_function(unsigned long key) const {
  auto it = frequency_map.find(key);
  if (it == frequency_map.end())
    return 0;
  return it->second;
}
