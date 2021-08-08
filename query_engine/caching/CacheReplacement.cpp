//
// Created by cristobal on 04-08-21.
//

#include "CacheReplacement.hpp"
#include "LRUReplacementStrategy.hpp"
#include <iostream>
#include <stdexcept>

template <class CRStrategy>
CacheReplacement<CRStrategy>::CacheReplacement(size_t max_size_allowed,
                                               I_DataManager *data_manager)
    : priority_set(StrategyWrapper(strategy)),
      max_size_allowed(max_size_allowed), size_used(0),
      data_manager(data_manager) {}
template <class CRStrategy>
bool CacheReplacement<CRStrategy>::hit_key(unsigned long key,
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
  unsigned long in_use_sz;
  {
    std::lock_guard lg(m);
    in_use_sz = in_use.size();
  }
  std::cerr << "priority set size: " << priority_set.size()
            << ", predicates in use " << in_use_sz
            << ", next_size: " << next_size
            << ", max_size_allowed: " << max_size_allowed << std::endl;

  while (next_size > max_size_allowed) {
    if (it == priority_set.end()) {
      std::cerr << "reached end, cant free more space for now" << std::endl;
      return false;
    }

    auto current_key = *it;

    if (is_using(current_key)) {
      it++;
      std::cerr << "using " << current_key << " trying next..." << std::endl;
      continue;
    }

    auto next_it = std::next(it);

    std::cerr << "going to drop " << current_key << std::endl;

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
  auto it = in_use.find(key);
  if (it == in_use.end()) {
    in_use[key] = 1;
    return;
  }
  it->second++;
}

template <class CRStrategy>
void CacheReplacement<CRStrategy>::mark_ready(unsigned long key) {
  auto it = in_use.find(key);
  if (it == in_use.end()) {
    return;
  }
  it->second--;
  if (it->second <= 0) {
    in_use.erase(it);
  }
}
template <class CRStrategy>
bool CacheReplacement<CRStrategy>::is_using(unsigned long key) {
  std::lock_guard lg(m);
  return in_use.find(key) != in_use.end();
}
template <class CRStrategy>
std::mutex &CacheReplacement<CRStrategy>::get_replacement_mutex() {
  return m;
}

template class CacheReplacement<LRUReplacementStrategy>;