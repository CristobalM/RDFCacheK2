//
// Created by cristobal on 3/5/22.
//

#include "FullyIndexedCache.hpp"
#include "FullyIndexedPredicate.hpp"

#include <iostream>

void FullyIndexedCache::init_streamer_predicates(
    const std::vector<unsigned long> &streamer_predicates) {
  for (auto predicate : streamer_predicates) {
    if (!should_cache(predicate)) {
      std::cout << "not caching predicate " << predicate << std::endl;
      continue;
    }

    auto metadata =
        pic.get_metadata_with_id(predicate);
    if (metadata)
      cache_replacement.hit_key(predicate, metadata->tree_size_in_memory);
  }
}

bool FullyIndexedCache::should_cache(unsigned long predicate) {
  auto fetch_result = pic.fetch_k2tree(predicate);
  if (!fetch_result.exists())
    return false;
  static constexpr auto max_to_cache_sz = 10'000'000UL;
  return fetch_result.get().size() < max_to_cache_sz;
}

FullyIndexedCacheResponse FullyIndexedCache::get(unsigned long predicate_id) {
  auto it = cached_predicates_sources.find(predicate_id);
  if (it == cached_predicates_sources.end())
    return FullyIndexedCacheResponse(nullptr);
  return FullyIndexedCacheResponse(it->second.get());
}

FullyIndexedCache::FullyIndexedCache(PredicatesIndexCacheMD &pic)
    : pic(pic), data_manager(cached_predicates_sources, pic),
      cache_replacement(1'000'000'000, &data_manager) {}

void FullyIndexedCache::CacheDataManager::remove_key(unsigned long key) {
  cache_map.erase(key);
}

void FullyIndexedCache::CacheDataManager::retrieve_key(unsigned long key) {
  auto fetch_result = pic.fetch_k2tree(key);
  cache_map[key] = std::make_unique<FullyIndexedPredicate>(fetch_result.get());
}
FullyIndexedCache::CacheDataManager::CacheDataManager(
    FullyIndexedCache::cache_map_t &cache_map, PredicatesIndexCacheMD &pic)
    : cache_map(cache_map), pic(pic) {}

void FullyIndexedCache::resync_predicate(unsigned long predicate_id) {
  // don't sync if it not currently loaded
  if (cached_predicates_sources.find(predicate_id) ==
      cached_predicates_sources.end())
    return;
  data_manager.retrieve_key(predicate_id);
}
