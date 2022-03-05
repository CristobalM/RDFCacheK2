//
// Created by cristobal on 3/2/22.
//

#ifndef RDFCACHEK2_CACHEDOBJECTSCANNER_HPP
#define RDFCACHEK2_CACHEDOBJECTSCANNER_HPP

#include "CachedSourceScanner.hpp"
#include "I_CachedPredicateSource.hpp"
class CachedObjectScanner : public CachedSourceScanner {

  const std::vector<unsigned long> &objects;
  int current_position;
  unsigned long subject_value;

public:
  CachedObjectScanner(I_CachedPredicateSource *cached_source,
                      unsigned long subject_value);
  bool has_next() override;
  std::pair<unsigned long, unsigned long> next() override;
};

#endif // RDFCACHEK2_CACHEDOBJECTSCANNER_HPP
