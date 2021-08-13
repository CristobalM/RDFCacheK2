//
// Created by cristobal on 05-08-21.
//

#include "CacheReplacementFactory.hpp"

#include "CacheReplacement.hpp"
#include "FrequencyReplacementStrategy.hpp"
#include "LRUReplacementStrategy.hpp"
#include "NoCachingReplacement.hpp"

std::unique_ptr<I_CacheReplacement>
CacheReplacementFactory::create_cache_replacement(
    unsigned long max_size_bytes, I_DataManager *cache_data_manager,
    I_CacheReplacement::REPLACEMENT_STRATEGY strategy) {
  switch (strategy) {
  case I_CacheReplacement::LRU:
    return std::make_unique<CacheReplacement<LRUReplacementStrategy>>(
        max_size_bytes, cache_data_manager);
  case I_CacheReplacement::REPLACEMENT_STRATEGY::FREQUENCY:
    return std::make_unique<CacheReplacement<FrequencyReplacementStrategy>>(
        max_size_bytes, cache_data_manager);
  case I_CacheReplacement::NO_CACHING:
  default:
    return std::make_unique<NoCachingReplacement>();
  }
}
