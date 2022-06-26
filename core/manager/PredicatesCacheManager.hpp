//
// Created by cristobal on 26-06-22.
//

#ifndef RDFCACHEK2_PREDICATESCACHEMANAGER_HPP
#define RDFCACHEK2_PREDICATESCACHEMANAGER_HPP

#include "fic/FullyIndexedCache.hpp"
#include "manager/I_DataManager.hpp"
#include "updating/I_DataMerger.hpp"
#include "updating/UpdatesLogger.hpp"

namespace k2cache {
struct PredicatesCacheManager : public I_DataManager, public I_DataMerger {
  virtual unsigned long get_predicate_size(unsigned long predicate_id) = 0;
  virtual void load_all_predicates() = 0;
  virtual FullyIndexedCache &get_fully_indexed_cache() = 0;
  virtual UpdatesLogger &get_updates_logger() = 0;
  virtual PredicatesIndexCacheMD &get_predicates_index_cache() = 0;
  virtual std::unique_ptr<K2TreeScanner> create_null_k2tree_scanner() = 0;
};
} // namespace k2cache

#endif // RDFCACHEK2_PREDICATESCACHEMANAGER_HPP
