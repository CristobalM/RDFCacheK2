//
// Created by cristobal on 04-08-21.
//

#include "LRUReplacementStrategy.hpp"

LRUReplacementStrategy::LRUReplacementStrategy() : low(0), high(0) {}
void LRUReplacementStrategy::hit_key(unsigned long key) {
  auto next_key_value = high++;
  remove_key(key);
  key_mapping_to_lru_value[key] = next_key_value;
}
void LRUReplacementStrategy::remove_key(unsigned long key) {
  auto it = key_mapping_to_lru_value.find(key);
  if (it == key_mapping_to_lru_value.end())
    return;
  auto lru_value = it->second;
  if (lru_value == low) {
    auto next = std::next(it);
    if (next != key_mapping_to_lru_value.end()) {
      low = next->second;
    } else {
      low = 0;
      high = 0;
    }
  }
  key_mapping_to_lru_value.erase(it);
}
long LRUReplacementStrategy::cost_function(unsigned long key) const {
  auto it = key_mapping_to_lru_value.find(key);
  long value;
  if (it == key_mapping_to_lru_value.end()) {
    value = high;
  } else {
    value = it->second;
  }
  return value - low; // protection against overflow
  // In practice this should never be a problem though, as 2^63 (longs) is
  // extremely large.
}
