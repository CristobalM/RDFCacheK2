//
// Created by cristobal on 3/2/22.
//

#ifndef RDFCACHEK2_CACHEDOBJECTSCANNER_HPP
#define RDFCACHEK2_CACHEDOBJECTSCANNER_HPP

#include "CachedSourceScanner.hpp"
#include "fic/I_CachedPredicateSource.hpp"
namespace k2cache {
class CachedObjectScanner : public CachedSourceScanner {

  const std::vector<uint64_t> &objects;
  int current_position;
  uint64_t subject_value;

public:
  CachedObjectScanner(I_CachedPredicateSource *cached_source,
                      uint64_t subject_value);
  bool has_next() override;
  std::pair<uint64_t, uint64_t> next() override;
};
} // namespace k2cache
#endif // RDFCACHEK2_CACHEDOBJECTSCANNER_HPP
