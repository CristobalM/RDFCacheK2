//
// Created by cristobal on 20-09-22.
//

#ifndef RDFCACHEK2_FULLYINDEXEDCACHE_HPP
#define RDFCACHEK2_FULLYINDEXEDCACHE_HPP
#include "FullyIndexedCacheResponse.hpp"
namespace k2cache {
struct FullyIndexedCache {
  virtual ~FullyIndexedCache() = default;
  virtual void init_streamer_predicates(
      const std::vector<uint64_t> &streamer_predicates) = 0;
  virtual bool should_cache(uint64_t predicate) = 0;
  virtual FullyIndexedCacheResponse get(uint64_t predicate_id) = 0;
  virtual void resync_predicate(uint64_t predicate_id) = 0;
};
} // namespace k2cache
#endif // RDFCACHEK2_FULLYINDEXEDCACHE_HPP
