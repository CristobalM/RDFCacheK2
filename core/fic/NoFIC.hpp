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
      const std::vector<unsigned long> &streamer_predicates) override;
  bool should_cache(unsigned long predicate) override;
  FullyIndexedCacheResponse get(unsigned long predicate_id) override;
  void resync_predicate(unsigned long predicate_id) override;
};
} // namespace k2cache

#endif // RDFCACHEK2_NOFIC_HPP
