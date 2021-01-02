#ifndef _CACHE_REPLACEMENT_FACTORY_HPP_
#define _CACHE_REPLACEMENT_FACTORY_HPP_

#include <cstddef>
#include "PredicatesCacheManager.hpp"

#include "CacheReplacement.hpp"

class CacheReplacementFactory{
  public:

  static CacheReplacement select_strategy(CacheReplacement::STRATEGY cache_replacement_strategy, 
  size_t memory_budget_bytes,
  std::shared_ptr<PredicatesCacheManager> &predicates_cache_manager);
};

#endif /* _CACHE_REPLACEMENT_FACTORY_HPP_ */
