//
// Created by cristobal on 3/2/22.
//

#ifndef RDFCACHEK2_CACHEDSUBJECTSCANNER_HPP
#define RDFCACHEK2_CACHEDSUBJECTSCANNER_HPP

#include "CachedSourceScanner.hpp"
#include "fic/I_CachedPredicateSource.hpp"
#include <array>
#include <utility>
#include <vector>
namespace k2cache {
class CachedSubjectScanner : public CachedSourceScanner {

  const std::vector<unsigned long> &subjects;
  int current_position;
  unsigned long object_value;

public:
  CachedSubjectScanner(I_CachedPredicateSource *cached_source,
                       unsigned long object_value);
  bool has_next() override;
  std::pair<unsigned long, unsigned long> next() override;
};
} // namespace k2cache
#endif // RDFCACHEK2_CACHEDSUBJECTSCANNER_HPP
