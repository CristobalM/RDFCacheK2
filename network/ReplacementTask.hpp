//
// Created by cristobal on 04-08-21.
//

#ifndef RDFCACHEK2_REPLACEMENTTASK_HPP
#define RDFCACHEK2_REPLACEMENTTASK_HPP

#include <Cache.hpp>
class ReplacementTask {
  Cache &cache;
  std::vector<unsigned long> predicates;

public:
  ReplacementTask(Cache &cache, std::vector<unsigned long> &&predicates);
  void process();
};

#endif // RDFCACHEK2_REPLACEMENTTASK_HPP
