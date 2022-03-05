//
// Created by cristobal on 3/2/22.
//

#ifndef RDFCACHEK2_CACHEDSOURCESCANNER_HPP
#define RDFCACHEK2_CACHEDSOURCESCANNER_HPP

#include <utility>

struct CachedSourceScanner {
  virtual ~CachedSourceScanner() = default;
  virtual bool has_next() = 0;
  virtual std::pair<unsigned long, unsigned long> next() = 0;
};
#endif // RDFCACHEK2_CACHEDSOURCESCANNER_HPP
