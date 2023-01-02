//
// Created by cristobal on 3/2/22.
//

#ifndef RDFCACHEK2_CACHEDSOURCESCANNER_HPP
#define RDFCACHEK2_CACHEDSOURCESCANNER_HPP

#include <utility>
#include <cstdint>

namespace k2cache {
struct CachedSourceScanner {
  virtual ~CachedSourceScanner() = default;
  virtual bool has_next() = 0;
  virtual std::pair<uint64_t, uint64_t> next() = 0;
};
} // namespace k2cache
#endif // RDFCACHEK2_CACHEDSOURCESCANNER_HPP
