//
// Created by cristobal on 3/5/22.
//

#ifndef RDFCACHEK2_FULLYINDEXEDCACHEIMPL_HPP
#define RDFCACHEK2_FULLYINDEXEDCACHEIMPL_HPP

#include "CachedPredicateSourceMap.hpp"
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

  std::unique_ptr<K2TreeFetcher> fetcher;
  std::unique_ptr<CachedPredicateSourceMap> cached_predicates_sources;
  std::unique_ptr<DataManager> data_manager;
  std::unique_ptr<I_CacheReplacement> cache_replacement;

//  fic::types::cache_map_t cached_predicates_sources;


//  class CacheDataManager : public DataManager {
//    cache_map_t &cache_map;
//    K2TreeFetcher &cdm_fetcher;
//
//  public:
//    CacheDataManager(cache_map_t &cache_map, K2TreeFetcher &f);
//    void remove_key(unsigned long key) override;
//    void retrieve_key(unsigned long key) override;
//  };

//  CacheDataManager data_manager;
//
//  CacheReplacement<LRUReplacementStrategy> cache_replacement;


public:
  FullyIndexedCacheImpl(
      std::unique_ptr<K2TreeFetcher> &&fetcher,
      std::unique_ptr<CachedPredicateSourceMap> &&cached_predicates_sources,
      std::unique_ptr<DataManager> &&data_manager,
      std::unique_ptr<I_CacheReplacement> &&cache_replacement
      );

  void init_streamer_predicates(
      const std::vector<unsigned long> &streamer_predicates) override;
  bool should_cache(unsigned long predicate) override;
  //  std::unique_ptr<I_CachedPredicateSource>
  //  make_cached_predicate_source(unsigned long predicate);
  FullyIndexedCacheResponse get(unsigned long predicate_id) override;
  void resync_predicate(unsigned long predicate_id) override;
};
} // namespace k2cache
#endif // RDFCACHEK2_FULLYINDEXEDCACHEIMPL_HPP
