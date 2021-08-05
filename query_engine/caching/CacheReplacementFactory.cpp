//
// Created by cristobal on 05-08-21.
//

#include "CacheReplacementFactory.hpp"

#include "CacheReplacement.hpp"
#include "LRUReplacementStrategy.hpp"
#include "NoCachingReplacement.hpp"

std::unique_ptr<I_CacheReplacement>
CacheReplacementFactory::create_cache_replacement(
    unsigned long max_size_bytes, I_DataManager *cache_data_manager,
    std::mutex &replacement_mutex, bool enable_caching) {
  if (enable_caching)
    return create_cache_replacement(
        max_size_bytes, cache_data_manager, replacement_mutex,
        I_CacheReplacement::REPLACEMENT_STRATEGY::LRU);

  return create_cache_replacement(
      max_size_bytes, cache_data_manager, replacement_mutex,
      I_CacheReplacement::REPLACEMENT_STRATEGY::NO_CACHING);
}
std::unique_ptr<I_CacheReplacement>
CacheReplacementFactory::create_cache_replacement(
    unsigned long max_size_bytes, I_DataManager *cache_data_manager,
    std::mutex &replacement_mutex,
    I_CacheReplacement::REPLACEMENT_STRATEGY strategy) {
  switch (strategy) {
  case I_CacheReplacement::NO_CACHING:
    return std::make_unique<NoCachingReplacement>();
  case I_CacheReplacement::LRU:
    return std::make_unique<CacheReplacement<LRUReplacementStrategy>>(
        max_size_bytes, cache_data_manager, replacement_mutex);
  }
}
