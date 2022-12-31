//
// Created by cristobal on 11-08-21.
//

#include "FrequencyReplacementStrategy.hpp"
#include "I_ReplacementPriorityQueue.hpp"

namespace k2cache {
FrequencyReplacementStrategy::FrequencyReplacementStrategy() = default;

void FrequencyReplacementStrategy::hit_key(uint64_t key) {
  auto it = frequency_map.find(key);
  if (it == frequency_map.end()) {
    frequency_map[key] = 1;
    return;
  }
  it->second++;
}

// we do nothing here, as hit frequency count shouldn't be affected
// when keys are discarded
void FrequencyReplacementStrategy::remove_key(uint64_t) {}
long FrequencyReplacementStrategy::cost_function(uint64_t key) const {
  auto it = frequency_map.find(key);
  if (it == frequency_map.end())
    return 0;
  return it->second;
}
bool FrequencyReplacementStrategy::should_discard_others_for_key(
    uint64_t key, I_ReplacementPriorityQueue &pq) {
  if (pq.empty())
    return false;

  auto traversal = pq.pq_traverse();
  auto required_size = pq.get_key_size(key);
  size_t accumulated = 0;

  auto key_cost = cost_function(key);

  while (traversal->has_next()) {
    auto current_key = traversal->next_key();
    auto current_key_size = pq.get_key_size(current_key);
    auto current_key_cost = cost_function(current_key);
    accumulated += current_key_size;
    if (key_cost < current_key_cost)
      return false;
    if (accumulated >= required_size) {
      return true;
    }
  }
  return false;
}
bool FrequencyReplacementStrategy::should_hit_without_retrieval(
    uint64_t, I_ReplacementPriorityQueue &) {
  return true;
}
} // namespace k2cache
