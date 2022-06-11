//
// Created by cristobal on 9/9/21.
//

#ifndef RDFCACHEK2_CACHEARGS_HPP
#define RDFCACHEK2_CACHEARGS_HPP

#include "replacement/I_CacheReplacement.hpp"
struct CacheArgs {
  std::string index_filename;
  unsigned long memory_budget_bytes;
  I_CacheReplacement::REPLACEMENT_STRATEGY replacement_strategy;
  std::string update_log_filename;
  std::string node_ids_filename;
};

#endif // RDFCACHEK2_CACHEARGS_HPP
