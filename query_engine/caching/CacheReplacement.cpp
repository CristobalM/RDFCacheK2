//
// Created by cristobal on 04-08-21.
//

#include "CacheReplacement.hpp"
#include "LRUReplacementStrategy.hpp"
#include <stdexcept>

template <class CRStrategy>
CacheReplacement<CRStrategy>::CacheReplacement(size_t max_size_allowed,
                                               I_DataManager *data_manager,
                                               std::mutex &replacement_mutex)
    : priority_set(StrategyWrapper(strategy)),
      max_size_allowed(max_size_allowed), size_used(0),
      data_manager(data_manager), replacement_mutex(replacement_mutex) {}
template <class CRStrategy>
bool CacheReplacement<CRStrategy>::hit_key(unsigned long key,
                                           size_t space_required) {
  if (space_required > max_size_allowed)
    return false;

  auto it = priority_set.find(key);

  if (it != priority_set.end()) {
    priority_set.erase(it);
    strategy.hit_key(key);
    priority_set.insert(key);
    return true;
  }

  auto next_size = space_required + size_used;
  it = priority_set.begin();
  while (next_size > max_size_allowed) {
    auto current_key = *it;

    if (it == priority_set.end()) {
      return false;
    }

    {
      std::lock_guard lg(replacement_mutex);
      if (in_use.find(current_key) != in_use.end()) {
        it++;
        continue;
      }
    }
    auto next_it = std::next(it);

    next_size -= space_map[current_key];
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
void CacheReplacement<CRStrategy>::mark_using(unsigned long key) {
  std::lock_guard lg(replacement_mutex);
  auto it = in_use.find(key);
  if (it == in_use.end()) {
    in_use[key] = 1;
    return;
  }
  it->second++;
}

template <class CRStrategy>
void CacheReplacement<CRStrategy>::mark_ready(unsigned long key) {
  std::lock_guard lg(replacement_mutex);
  auto it = in_use.find(key);
  if (it == in_use.end()) {
    return;
  }
  it->second--;
  if (it->second <= 0) {
    in_use.erase(it);
  }
}

template class CacheReplacement<LRUReplacementStrategy>;