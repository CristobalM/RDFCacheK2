#ifndef _LRU_CACHE_REPLACEMENT_HPP_
#define _LRU_CACHE_REPLACEMENT_HPP_

#include <cstddef>

#include "CacheReplacement.hpp"
#include "PredicatesCacheManager.hpp"

class LRUCacheReplacement : public CacheReplacement {

  size_t memory_budget_bytes;
  std::shared_ptr<PredicatesCacheManager> predicates_cache_manager;

  public:

  LRUCacheReplacement(size_t memory_budget_bytes, 
  std::shared_ptr<PredicatesCacheManager> &predicates_cache_manager);
  
};

#endif /* _LRU_CACHE_REPLACEMENT_HPP_ */

