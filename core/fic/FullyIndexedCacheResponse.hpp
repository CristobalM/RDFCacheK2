//
// Created by cristobal on 3/5/22.
//

#ifndef RDFCACHEK2_FULLYINDEXEDCACHERESPONSE_HPP
#define RDFCACHEK2_FULLYINDEXEDCACHERESPONSE_HPP

#include "I_CachedPredicateSource.hpp"
class FullyIndexedCacheResponse {
  I_CachedPredicateSource *source;

public:
  explicit FullyIndexedCacheResponse(I_CachedPredicateSource *source);
  bool exists();
  I_CachedPredicateSource *get();
};

#endif // RDFCACHEK2_FULLYINDEXEDCACHERESPONSE_HPP
