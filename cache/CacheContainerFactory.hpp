//
// Created by cristobal on 26-06-22.
//

#ifndef RDFCACHEK2_CACHECONTAINERFACTORY_HPP
#define RDFCACHEK2_CACHECONTAINERFACTORY_HPP
#include "CacheArgs.hpp"
#include "CacheContainer.hpp"
#include <memory>
namespace k2cache {

struct CacheContainerFactory {
  static std::unique_ptr<CacheContainer> create(const CacheArgs &args);
};
} // namespace k2cache
#endif // RDFCACHEK2_CACHECONTAINERFACTORY_HPP
