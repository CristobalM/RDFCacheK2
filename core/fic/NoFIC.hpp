//
// Created by cristobal on 20-09-22.
//

#ifndef RDFCACHEK2_NOFIC_HPP
#define RDFCACHEK2_NOFIC_HPP
#include "FullyIndexedCache.hpp"
namespace k2cache {
class NoFIC : public FullyIndexedCache {
public:
  void init_streamer_predicates(
      const std::vector<uint64_t> &streamer_predicates) override;
  bool should_cache(uint64_t predicate) override;
  FullyIndexedCacheResponse get(uint64_t predicate_id) override;
  void resync_predicate(uint64_t predicate_id) override;
};
} // namespace k2cache

#endif // RDFCACHEK2_NOFIC_HPP
