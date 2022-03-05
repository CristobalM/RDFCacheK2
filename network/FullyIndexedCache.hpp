//
// Created by cristobal on 3/5/22.
//

#ifndef RDFCACHEK2_FULLYINDEXEDCACHE_HPP
#define RDFCACHEK2_FULLYINDEXEDCACHE_HPP

#include "Cache.hpp"
#include "FullyIndexedCacheResponse.hpp"
#include "I_CachedPredicateSource.hpp"
#include "replacement/CacheReplacement.hpp"
#include "replacement/LRUReplacementStrategy.hpp"
#include <memory>
#include <unordered_map>

class FullyIndexedCache {

  Cache &cache;

  using cache_map_t =
      std::unordered_map<unsigned long,
                         std::unique_ptr<I_CachedPredicateSource>>;

  cache_map_t cached_predicates_sources;

  class CacheDataManager : public I_DataManager {
    cache_map_t &cache_map;
    Cache &cache;

  public:
    CacheDataManager(cache_map_t &cache_map, Cache &cache);
    void remove_key(unsigned long key) override;
    void retrieve_key(unsigned long key) override;
  };

  CacheDataManager data_manager;

  CacheReplacement<LRUReplacementStrategy> cache_replacement;

public:
  explicit FullyIndexedCache(Cache &cache);

  void init_streamer_predicates(
      const std::vector<unsigned long> &streamer_predicates);
  bool should_cache(unsigned long predicate);
  //  std::unique_ptr<I_CachedPredicateSource>
  //  make_cached_predicate_source(unsigned long predicate);
  FullyIndexedCacheResponse get(unsigned long predicate_id);
};

#endif // RDFCACHEK2_FULLYINDEXEDCACHE_HPP
