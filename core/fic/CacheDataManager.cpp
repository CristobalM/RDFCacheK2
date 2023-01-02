//
// Created by cristobal on 03-10-22.
//

#include "CacheDataManager.hpp"
#include "FullyIndexedPredicate.hpp"

namespace k2cache {
void CacheDataManager::remove_key(uint64_t key) {
    cache_map.erase(key);
}
void CacheDataManager::retrieve_key(uint64_t key) {
    auto fetch_result = cdm_fetcher.fetch_k2tree(key);
    cache_map[key] = std::make_unique<FullyIndexedPredicate>(fetch_result.get());
}
CacheDataManager::CacheDataManager(fic::types::cache_map_t &cache_map,
                                   K2TreeFetcher &f)
    : cache_map(cache_map), cdm_fetcher(f) {}
} // namespace k2cache
