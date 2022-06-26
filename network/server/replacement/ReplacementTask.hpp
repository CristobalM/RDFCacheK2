//
// Created by cristobal on 04-08-21.
//

#ifndef RDFCACHEK2_REPLACEMENTTASK_HPP
#define RDFCACHEK2_REPLACEMENTTASK_HPP

#include "Cache.hpp"
namespace k2cache {
class ReplacementTask {
  Cache &cache;
  std::shared_ptr<const std::vector<unsigned long>> predicates;

public:
  ReplacementTask(Cache &cache,
                  std::shared_ptr<const std::vector<unsigned long>> predicates);
  void process();
};
} // namespace k2cache
#endif // RDFCACHEK2_REPLACEMENTTASK_HPP
