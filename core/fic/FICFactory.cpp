//
// Created by cristobal on 03-10-22.
//

#include "FICFactory.hpp"
#include "CacheDataManager.hpp"

namespace k2cache::FICFactory {

std::unique_ptr<FullyIndexedCache>
create(std::unique_ptr<K2TreeFetcher> &&fetcher) {
  fic::types::cache_map_t m;

  auto data_manager = std::make_unique<CacheDataManager>(m,*fetcher);


  return std::make_unique<FullyIndexedCache>(
      std::move(fetcher),
      std::move(data_manager),
      );
}
}