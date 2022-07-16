//
// Created by cristobal on 05-08-21.
//

#ifndef RDFCACHEK2_CACHEREPLACEMENTFACTORY_HPP
#define RDFCACHEK2_CACHEREPLACEMENTFACTORY_HPP

#include "I_CacheReplacement.hpp"
#include "manager/DataManager.hpp"
#include <memory>
#include <mutex>
namespace k2cache {
class CacheReplacementFactory {
public:
  static std::unique_ptr<I_CacheReplacement>
  create_cache_replacement(unsigned long max_size_bytes,
                           DataManager *cache_data_manager,
                           I_CacheReplacement::REPLACEMENT_STRATEGY strategy);
};
} // namespace k2cache

#endif // RDFCACHEK2_CACHEREPLACEMENTFACTORY_HPP
