//
// Created by cristobal on 11-08-21.
//

#include "FrequencyReplacementStrategy.hpp"
FrequencyReplacementStrategy::FrequencyReplacementStrategy() = default;
bool FrequencyReplacementStrategy::operator()(unsigned long lhs,
                                              unsigned long rhs) const {
  auto left_frequency = get_frequency(lhs);
  auto right_frequency = get_frequency(rhs);
  // This operator is also used to check for equality,
  // with only frequency comparation, different keys could be treated as the
  // same so we do this previous check to avoid that
  if (left_frequency == right_frequency)
    return lhs < rhs;
  return left_frequency < right_frequency;
}
void FrequencyReplacementStrategy::hit_key(unsigned long key) {
  auto it = frequency_map.find(key);
  if (it == frequency_map.end()) {
    frequency_map[key] = 1;
    return;
  }
  it->second++;
}
void FrequencyReplacementStrategy::remove_key(unsigned long) {}
long FrequencyReplacementStrategy::get_frequency(unsigned long key) const {
  auto it = frequency_map.find(key);
  if (it == frequency_map.end())
    return 0;
  return it->second;
}
