#ifndef _LRU_CACHE_REPLACEMENT_HPP_
#define _LRU_CACHE_REPLACEMENT_HPP_

#include <cstddef>

#include "CacheReplacement.hpp"
#include "PredicatesCacheManager.hpp"
#include "LRU/LRUController.hpp"
#include "LRU/LRUQueue.hpp"

class LRUCacheReplacement : public CacheReplacement, public LRUController {

  size_t memory_budget_bytes;
  std::shared_ptr<PredicatesCacheManager> predicates_cache_manager;

  LRUQueue lru_queue;

  public:

  LRUCacheReplacement(size_t memory_budget_bytes, 
  std::shared_ptr<PredicatesCacheManager> &predicates_cache_manager);

  void retrieve_element(unsigned long element_id) override;
  void discard_element(unsigned long element_id) override;

  unsigned long get_max_size_bytes() override;
  
};

#endif /* _LRU_CACHE_REPLACEMENT_HPP_ */

