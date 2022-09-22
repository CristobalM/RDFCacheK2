//
// Created by cristobal on 3/5/22.
//

#include "FullyIndexedCacheImpl.hpp"
#include "FullyIndexedPredicate.hpp"

#include <iostream>

namespace k2cache {
void FullyIndexedCacheImpl::init_streamer_predicates(
    const std::vector<unsigned long> &streamer_predicates) {
  for (auto predicate : streamer_predicates) {
    if (!should_cache(predicate)) {
      std::cout << "not caching predicate " << predicate << std::endl;
      continue;
    }

    auto predicate_size = fetcher->fetch_k2tree(predicate).get().size();
    // 25 bytes per point is the worst case scenario for 100k points (random
    // distribution), so it works as an upper bound approximation
    cache_replacement.hit_key(predicate, predicate_size * 25);
  }
}

bool FullyIndexedCacheImpl::should_cache(unsigned long predicate) {
  auto fetch_result = fetcher->fetch_k2tree_if_loaded(predicate);
  if (!fetch_result.loaded())
    return false;
  static constexpr auto max_to_cache_sz = 10'000'000UL;
  return fetch_result.get().size() < max_to_cache_sz;
}

FullyIndexedCacheResponse
FullyIndexedCacheImpl::get(unsigned long predicate_id) {
  auto it = cached_predicates_sources.find(predicate_id);
  if (it == cached_predicates_sources.end())
    return FullyIndexedCacheResponse(nullptr);
  return FullyIndexedCacheResponse(it->second.get());
}

FullyIndexedCacheImpl::FullyIndexedCacheImpl(
    std::unique_ptr<K2TreeFetcher> &&fetcher)
    : fetcher(std::move(fetcher)),
      data_manager(cached_predicates_sources, *this->fetcher),
      cache_replacement(1'000'000'000, &data_manager) {}

void FullyIndexedCacheImpl::CacheDataManager::remove_key(unsigned long key) {
  cache_map.erase(key);
}

void FullyIndexedCacheImpl::CacheDataManager::retrieve_key(unsigned long key) {
  auto fetch_result = cdm_fetcher.fetch_k2tree(key);
  cache_map[key] = std::make_unique<FullyIndexedPredicate>(fetch_result.get());
}
FullyIndexedCacheImpl::CacheDataManager::CacheDataManager(
    FullyIndexedCacheImpl::cache_map_t &cache_map, K2TreeFetcher &f)
    : cache_map(cache_map), cdm_fetcher(f) {}

void FullyIndexedCacheImpl::resync_predicate(unsigned long predicate_id) {
  // don't sync if it not currently loaded
  if (cached_predicates_sources.find(predicate_id) ==
      cached_predicates_sources.end())
    return;
  data_manager.retrieve_key(predicate_id);
}
} // namespace k2cache