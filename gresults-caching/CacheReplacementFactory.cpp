#include <string>

#include "CacheReplacementFactory.hpp"
#include "LRUCacheReplacement.hpp"

CacheReplacement CacheReplacementFactory::select_strategy(
    CacheReplacement::STRATEGY cache_replacement_strategy,
    size_t memory_budget_bytes,
    std::shared_ptr<PredicatesCacheManager> &predicates_cache_manager) {
  switch (cache_replacement_strategy) {
  case CacheReplacement::STRATEGY::LRU:
    return LRUCacheReplacement(memory_budget_bytes, predicates_cache_manager);
    break;
  default:
    throw "Invalid strategy selected " +
        std::to_string(cache_replacement_strategy);
  }
}
