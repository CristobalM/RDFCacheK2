//
// Created by cristobal on 03-10-22.
//

#ifndef RDFCACHEK2_CACHEDATAMANAGER_HPP
#define RDFCACHEK2_CACHEDATAMANAGER_HPP

#include "manager/DataManager.hpp"
#include "manager/K2TreeFetcher.hpp"
#include "types.hpp"
namespace k2cache{
class CacheDataManager : public DataManager {
      fic::types::cache_map_t &cache_map;
      K2TreeFetcher &cdm_fetcher;

public:
  void remove_key(uint64_t key) override;
  void retrieve_key(uint64_t key) override;

  CacheDataManager(fic::types::cache_map_t &cache_map, K2TreeFetcher &f);
};
}


#endif // RDFCACHEK2_CACHEDATAMANAGER_HPP
