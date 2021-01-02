#ifndef _CACHE_REPLACEMENT_FACTORY_HPP_
#define _CACHE_REPLACEMENT_FACTORY_HPP_

#include <cstdint>

#include "CacheReplacement.hpp"

class CacheReplacementFactory{
  public:

  static CacheReplacement select_strategy(CacheReplacement::STRATEGY cache_replacement_strategy, size_t memory_budget_bytes);
};

#endif /* _CACHE_REPLACEMENT_FACTORY_HPP_ */
