//
// Created by cristobal on 04-08-21.
//

#include "LRUReplacementStrategy.hpp"
bool LRUReplacementStrategy::operator()(unsigned long lhs,
                                        unsigned long rhs) const {
  auto left_it = key_mapping_to_lru_value.find(lhs);
  auto right_it = key_mapping_to_lru_value.find(rhs);
  long left;
  long right;
  if (left_it == key_mapping_to_lru_value.end())
    left = high;
  else
    left = left_it->second;

  if (right_it == key_mapping_to_lru_value.end())
    right = high;
  else
    right = right_it->second;

  auto priority_value_left = left - low;
  auto priority_value_right = right - low;
  return priority_value_left <
         priority_value_right; // protection against overflow
  // priority queue must be in ascending order, new values will take higher
  // values, so to comply with lru retrieve for deletion lower values
}
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
