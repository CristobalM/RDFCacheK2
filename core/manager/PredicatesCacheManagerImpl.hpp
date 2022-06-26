//
// Created by Cristobal Miranda, 2020
//

#ifndef RDFCACHEK2_PREDICATESCACHEMANAGERIMPL_HPP
#define RDFCACHEK2_PREDICATESCACHEMANAGERIMPL_HPP

#include <functional>
#include <istream>
#include <memory>
#include <string>

#include "CacheArgs.hpp"
#include "DataManager.hpp"
#include "I_FileRWHandler.hpp"
#include "PredicatesCacheManager.hpp"
#include "PredicatesIndexCacheMD.hpp"
#include "fic/FullyIndexedCache.hpp"
#include "k2tree/K2TreeBulkOp.hpp"
#include "k2tree/K2TreeMixed.hpp"
#include "k2tree/K2TreeScanner.hpp"
#include "manager/DataManager.hpp"
#include "updating/DataMerger.hpp"
#include "updating/K2TreeUpdates.hpp"
#include "updating/UpdatesLogger.hpp"
#include "updating/UpdatesLoggerFilesManager.hpp"

namespace k2cache {
class PredicatesCacheManagerImpl : public PredicatesCacheManager {
  std::unique_ptr<PredicatesIndexCacheMD> predicates_index;
  std::unique_ptr<UpdatesLogger> updates_logger;

  FullyIndexedCache fully_indexed_cache;

public:
  // read/write constructors
  PredicatesCacheManagerImpl(
      std::unique_ptr<PredicatesIndexCacheMD> &&predicates_index,
      std::unique_ptr<UpdatesLogger> &&update_logger);

  PredicatesCacheManagerImpl(
      std::unique_ptr<I_FileRWHandler> &&index_file_handler,
      UpdatesLoggerFilesManager &&updates_logger_fm);

  // read only constructor
  explicit PredicatesCacheManagerImpl(
      std::unique_ptr<PredicatesIndexCacheMD> &&predicates_index);

  PredicatesIndexCacheMD &get_predicates_index_cache() override;
  void load_all_predicates() override;

  size_t get_predicate_size(unsigned long predicate_id) override;
  void remove_key(unsigned long key) override;
  void retrieve_key(unsigned long key) override;

  std::unique_ptr<K2TreeScanner> create_null_k2tree_scanner() override;

  void merge_add_tree(unsigned long predicate_id, K2TreeMixed &k2tree) override;

  void merge_delete_tree(unsigned long predicate_id,
                         K2TreeMixed &k2tree) override;

  void merge_update(std::vector<K2TreeUpdates> &updates) override;

  //  explicit PredicatesCacheManagerImpl(const std::string
  //  &input_k2tree_filename);

  FullyIndexedCache &get_fully_indexed_cache() override;

  UpdatesLogger &get_updates_logger() override;

private:
  void merge_op_tree(unsigned long predicate_id, K2TreeMixed &to_merge_k2tree,
                     const std::function<void(K2TreeBulkOp &, unsigned long,
                                              unsigned long)> &op,
                     bool create_if_doesnt_exists);
};
} // namespace k2cache

#endif // RDFCACHEK2_PREDICATESCACHEMANAGERIMPL_HPP
