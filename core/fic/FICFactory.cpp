//
// Created by cristobal on 03-10-22.
//

#include "FICFactory.hpp"
#include "CacheDataManager.hpp"
#include "FullyIndexedCacheImpl.hpp"

namespace k2cache::FICFactory {

std::unique_ptr<FullyIndexedCache> create(K2TreeFetcher &idx) {
  auto m = std::make_unique<fic::types::cache_map_t>();

  auto data_manager = std::make_unique<CacheDataManager>(*m, idx);

  auto cache_replacement =
      std::make_unique<CacheReplacement<LRUReplacementStrategy>>(
          1'000'000'000, data_manager.get());

  return std::make_unique<FullyIndexedCacheImpl>(
      idx, std::move(m), std::move(data_manager), std::move(cache_replacement));
}
} // namespace k2cache::FICFactory