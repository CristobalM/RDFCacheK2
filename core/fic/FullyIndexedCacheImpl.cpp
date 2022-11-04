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

    auto predicate_size = fetcher.fetch_k2tree(predicate).get().size();
    // 25 bytes per point is the worst case scenario for 100k points (random
    // distribution), so it works as an upper bound approximation
    cache_replacement->hit_key(predicate, predicate_size * 25);
  }
}

bool FullyIndexedCacheImpl::should_cache(unsigned long predicate) {
  auto fetch_result = fetcher.fetch_k2tree_if_loaded(predicate);
  if (!fetch_result.loaded())
    return false;
  static constexpr auto max_to_cache_sz = 10'000'000UL;
  return fetch_result.get().size() < max_to_cache_sz;
}

FullyIndexedCacheResponse
FullyIndexedCacheImpl::get(unsigned long predicate_id) {
  auto it = cached_predicates_sources->find(predicate_id);
  if (it == cached_predicates_sources->end())
    return FullyIndexedCacheResponse(nullptr);
  return FullyIndexedCacheResponse(it->second.get());
}

FullyIndexedCacheImpl::FullyIndexedCacheImpl(
    K2TreeFetcher &fetcher,
    std::unique_ptr<fic::types::cache_map_t> &&cached_predicates_sources,
    std::unique_ptr<DataManager> &&data_manager,
    std::unique_ptr<I_CacheReplacement> &&cache_replacement)
    : fetcher(fetcher),
      cached_predicates_sources(std::move(cached_predicates_sources)),
      data_manager(std::move(data_manager)),
      cache_replacement(std::move(cache_replacement)) {}

void FullyIndexedCacheImpl::resync_predicate(unsigned long predicate_id) {
  // don't sync if it not currently loaded
  if (cached_predicates_sources->find(predicate_id) ==
      cached_predicates_sources->end())
    return;
  data_manager->retrieve_key(predicate_id);
}
} // namespace k2cache