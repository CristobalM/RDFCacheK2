//
// Created by cristobal on 3/5/22.
//

#ifndef RDFCACHEK2_FULLYINDEXEDCACHEIMPL_HPP
#define RDFCACHEK2_FULLYINDEXEDCACHEIMPL_HPP

#include "FullyIndexedCache.hpp"
#include "FullyIndexedCacheResponse.hpp"
#include "I_CachedPredicateSource.hpp"
#include "manager/DataManager.hpp"
#include "manager/K2TreeFetcher.hpp"
#include "replacement/CacheReplacement.hpp"
#include "replacement/LRUReplacementStrategy.hpp"
#include "types.hpp"

namespace k2cache {
class FullyIndexedCacheImpl : public FullyIndexedCache {

  K2TreeFetcher &fetcher;
  std::unique_ptr<fic::types::cache_map_t> cached_predicates_sources;
  std::unique_ptr<DataManager> data_manager;
  std::unique_ptr<I_CacheReplacement> cache_replacement;

public:
  FullyIndexedCacheImpl(
      K2TreeFetcher &fetcher,
      std::unique_ptr<fic::types::cache_map_t> &&cached_predicates_sources,
      std::unique_ptr<DataManager> &&data_manager,
      std::unique_ptr<I_CacheReplacement> &&cache_replacement);

  void init_streamer_predicates(
      const std::vector<uint64_t> &streamer_predicates) override;
  bool should_cache(uint64_t predicate) override;
  FullyIndexedCacheResponse get(uint64_t predicate_id) override;
  void resync_predicate(uint64_t predicate_id) override;
};
} // namespace k2cache
#endif // RDFCACHEK2_FULLYINDEXEDCACHEIMPL_HPP
