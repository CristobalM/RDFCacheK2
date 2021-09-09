//
// Created by cristobal on 9/6/21.
//

#ifndef RDFCACHEK2_CACHEARGS_HPP
#define RDFCACHEK2_CACHEARGS_HPP

#include <caching/I_CacheReplacement.hpp>
struct CacheArgs {

  unsigned long memory_budget_bytes;
  std::string temp_files_dir;
  unsigned long time_out_ms;
  I_CacheReplacement::REPLACEMENT_STRATEGY replacement_strategy;
  std::string update_log_filename;
};

#endif // RDFCACHEK2_CACHEARGS_HPP
