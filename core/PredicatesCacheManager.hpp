//
// Created by Cristobal Miranda, 2020
//

#ifndef RDFCACHEK2_PREDICATESCACHEMANAGER_HPP
#define RDFCACHEK2_PREDICATESCACHEMANAGER_HPP

#include <functional>
#include <istream>
#include <memory>
#include <string>

#include "CacheArgs.hpp"
#include "FullyIndexedCache.hpp"
#include "I_DataManager.hpp"
#include "I_DataMerger.hpp"
#include "I_FileRWHandler.hpp"
#include "I_UpdateLoggerPCM.hpp"
#include "K2TreeBulkOp.hpp"
#include "K2TreeMixed.hpp"
#include "K2TreeScanner.hpp"
#include "K2TreeUpdates.hpp"
#include "PredicatesIndexCacheMD.hpp"
#include "UpdatesLogger.hpp"
#include "UpdatesLoggerFilesManager.hpp"

class PredicatesCacheManager : public I_DataManager, public I_DataMerger {
  std::unique_ptr<PredicatesIndexCacheMD> predicates_index;
  std::unique_ptr<UpdatesLogger> updates_logger;

  FullyIndexedCache fully_indexed_cache;

public:
  // read/write constructors
  PredicatesCacheManager(
      std::unique_ptr<PredicatesIndexCacheMD> &&predicates_index,
      std::unique_ptr<UpdatesLogger> &&update_logger);

  PredicatesCacheManager(std::unique_ptr<I_FileRWHandler> &&index_file_handler,
                         UpdatesLoggerFilesManager &&updates_logger_fm);

  explicit PredicatesCacheManager(const CacheArgs &cache_args);

  // read only constructors

  explicit PredicatesCacheManager(
      std::unique_ptr<PredicatesIndexCacheMD> &&predicates_index);


  explicit PredicatesCacheManager(
      std::unique_ptr<I_FileRWHandler> &&index_file_handler);

  explicit PredicatesCacheManager(const std::string &index_filename);


  PredicatesIndexCacheMD &get_predicates_index_cache();
  void load_all_predicates();

  size_t get_predicate_size(unsigned long predicate_id);
  void remove_key(unsigned long key) override;
  void retrieve_key(unsigned long key) override;

  std::unique_ptr<K2TreeScanner> create_null_k2tree_scanner();

  void merge_add_tree(unsigned long predicate_id, K2TreeMixed &k2tree) override;

  void merge_delete_tree(unsigned long predicate_id,
                         K2TreeMixed &k2tree) override;

  void set_update_logger(I_UpdateLoggerPCM *input_update_logger);

  void merge_update(std::vector<K2TreeUpdates> &updates) override;

  //  explicit PredicatesCacheManager(const std::string &input_k2tree_filename);

  FullyIndexedCache &get_fully_indexed_cache();

  UpdatesLogger &get_updates_logger();

private:
  void merge_op_tree(unsigned long predicate_id, K2TreeMixed &to_merge_k2tree,
                     const std::function<void(K2TreeBulkOp &, unsigned long,
                                              unsigned long)> &op,
                     bool create_if_doesnt_exists);
};

#endif // RDFCACHEK2_PREDICATESCACHEMANAGER_HPP
