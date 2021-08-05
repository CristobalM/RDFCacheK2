//
// Created by cristobal on 05-08-21.
//

#ifndef RDFCACHEK2_CACHEREPLACEMENTFACTORY_HPP
#define RDFCACHEK2_CACHEREPLACEMENTFACTORY_HPP

#include "I_CacheReplacement.hpp"
#include <I_DataManager.hpp>
#include <memory>
#include <mutex>
class CacheReplacementFactory {
public:
  static std::unique_ptr<I_CacheReplacement>
  create_cache_replacement(unsigned long max_size_bytes,
                           I_DataManager *cache_data_manager,
                           std::mutex &replacement_mutex, bool enable_caching);
  static std::unique_ptr<I_CacheReplacement>
  create_cache_replacement(unsigned long max_size_bytes,
                           I_DataManager *cache_data_manager,
                           std::mutex &replacement_mutex,
                           I_CacheReplacement::REPLACEMENT_STRATEGY strategy);
};

#endif // RDFCACHEK2_CACHEREPLACEMENTFACTORY_HPP
