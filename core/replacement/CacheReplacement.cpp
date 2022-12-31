//
// Created by cristobal on 04-08-21.
//

#include "CacheReplacement.hpp"
#include "FrequencyReplacementStrategy.hpp"
#include "I_PQTraverse.hpp"
#include "LRUReplacementStrategy.hpp"
#include <iostream>
#include <memory>
#include <stdexcept>

namespace k2cache {
template <class CRStrategy>
CacheReplacement<CRStrategy>::CacheReplacement(size_t max_size_allowed,
                                               DataManager *data_manager)
    : priority_set(StrategyWrapper(strategy)),
      cr_priority_queue(priority_set, space_map),
      max_size_allowed(max_size_allowed), size_used(0),
      data_manager(data_manager) {}
template <class CRStrategy>
bool CacheReplacement<CRStrategy>::hit_key(uint64_t key,
                                           size_t space_required) {
  if (space_required > max_size_allowed) {
    std::cerr << "ignoring " << key << " because its size " << space_required
              << " > max_allowed=" << max_size_allowed << std::endl;
    return false;
  }

  auto it = priority_set.find(key);

  if (it != priority_set.end()) {
    priority_set.erase(it);
    strategy.hit_key(key);
    priority_set.insert(key);
    return true;
  }

  auto next_size = space_required + size_used;
  it = priority_set.begin();
  uint64_t in_use_sz;
  {
    std::lock_guard lg(m);
    in_use_sz = in_use.size();
  }
  std::cerr << "priority set size: " << priority_set.size()
            << ", predicates in use " << in_use_sz
            << ", next_size: " << next_size
            << ", max_size_allowed: " << max_size_allowed << std::endl;
  size_t size_erased = 0;

  if (next_size > max_size_allowed &&
      !strategy.should_discard_others_for_key(key, cr_priority_queue)) {

    if (strategy.should_hit_without_retrieval(key, cr_priority_queue))
      strategy.hit_key(key);

    return false;
  }

  while (next_size > max_size_allowed) {
    if (it == priority_set.end()) {
      std::cerr << "reached end, cant free more space for now" << std::endl;
      size_used -= size_erased;
      return false;
    }

    auto current_key = *it;

    if (is_using(current_key)) {
      it++;
      std::cerr << "using " << current_key << " trying next..." << std::endl;
      continue;
    }

    auto next_it = std::next(it);

    auto erasing_sz = space_map[current_key];
    std::cerr << "going to drop " << current_key << " with size " << erasing_sz
              << std::endl;

    size_erased += erasing_sz;
    next_size -= erasing_sz;
    space_map.erase(current_key);

    priority_set.erase(it);
    strategy.remove_key(current_key);
    data_manager->remove_key(current_key);
    it = next_it;
  }
  size_used = next_size;
  space_map[key] = space_required;
  data_manager->retrieve_key(key);
  strategy.hit_key(key);
  priority_set.insert(key);
  return true;
}
template <class CRStrategy>
void CacheReplacement<CRStrategy>::mark_using(uint64_t key) {
  auto it = in_use.find(key);
  if (it == in_use.end()) {
    in_use[key] = 1;
    return;
  }
  it->second++;
}

template <class CRStrategy>
void CacheReplacement<CRStrategy>::mark_ready(uint64_t key) {
  auto it = in_use.find(key);
  if (it == in_use.end()) {
    return;
  }
  it->second = it->second > 0 ? it->second - 1 : 0;
  if (it->second <= 0) {
    in_use.erase(it);
  }
}
template <class CRStrategy>
bool CacheReplacement<CRStrategy>::is_using(uint64_t key) {
  std::lock_guard lg(m);
  return in_use.find(key) != in_use.end();
}
template <class CRStrategy>
std::mutex &CacheReplacement<CRStrategy>::get_replacement_mutex() {
  return m;
}

template <class CRStrategy>
bool CacheReplacement<CRStrategy>::StrategyWrapper::operator()(
    uint64_t lhs, uint64_t rhs) const {
  // return strategy.operator()(lhs, rhs);
  auto left = strategy.cost_function(lhs);
  auto right = strategy.cost_function(rhs);
  if (left == right)
    return lhs < rhs;
  return left < right;
}
template <class CRStrategy>
CacheReplacement<CRStrategy>::StrategyWrapper::StrategyWrapper(
    CRStrategy &strategy)
    : strategy(strategy) {}

template <class CRStrategy>
CacheReplacement<CRStrategy>::CRPriorityQueue::CRPriorityQueue(
    CacheReplacement::pq_t &priority_queue,
    CacheReplacement::space_map_t &space_map)
    : priority_queue(priority_queue), space_map(space_map) {}
template <class CRStrategy>
bool CacheReplacement<CRStrategy>::CRPriorityQueue::empty() {
  return priority_queue.empty();
}
template <class CRStrategy>
std::unique_ptr<I_PQTraverse>
CacheReplacement<CRStrategy>::CRPriorityQueue::pq_traverse() {
  return std::make_unique<CRPQTraverse>(priority_queue);
}
template <class CRStrategy>
size_t
CacheReplacement<CRStrategy>::CRPriorityQueue::get_key_size(uint64_t key) {
  return space_map[key];
}

template <class CRStrategy>
bool CacheReplacement<CRStrategy>::CRPQTraverse::has_next() {
  return it != pq.end();
}
template <class CRStrategy>
uint64_t CacheReplacement<CRStrategy>::CRPQTraverse::next_key() {
  auto result = *it;
  it++;
  return result;
}
template <class CRStrategy>
CacheReplacement<CRStrategy>::CRPQTraverse::CRPQTraverse(
    CacheReplacement::pq_t &pq)
    : pq(pq), it(pq.begin()) {}

template class CacheReplacement<LRUReplacementStrategy>;
template class CacheReplacement<FrequencyReplacementStrategy>;
} // namespace k2cache
