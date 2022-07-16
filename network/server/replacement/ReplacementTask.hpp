//
// Created by cristobal on 04-08-21.
//

#ifndef RDFCACHEK2_REPLACEMENTTASK_HPP
#define RDFCACHEK2_REPLACEMENTTASK_HPP

#include <memory>
#include <vector>

#include "CacheContainer.hpp"

namespace k2cache {
class ReplacementTask {
  CacheContainer &cache;
  std::shared_ptr<const std::vector<unsigned long>> predicates;

public:
  ReplacementTask(CacheContainer &cache,
                  std::shared_ptr<const std::vector<unsigned long>> predicates);
  void process();
};
} // namespace k2cache
#endif // RDFCACHEK2_REPLACEMENTTASK_HPP
