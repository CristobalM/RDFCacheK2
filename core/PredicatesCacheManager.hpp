//
// Created by Cristobal Miranda, 2020
//

#ifndef RDFCACHEK2_PREDICATESCACHEMANAGER_HPP
#define RDFCACHEK2_PREDICATESCACHEMANAGER_HPP

#include "I_DataManager.hpp"
#include "I_UpdateLoggerPCM.hpp"
#include "K2TreeBulkOp.hpp"
#include "NodeId.hpp"
#include "PredicatesIndexCacheMD.hpp"
#include "RDFTriple.hpp"
#include <K2TreeUpdates.hpp>
#include <functional>
#include <istream>
#include <memory>
#include <string>

class PredicatesCacheManager : public I_DataManager {
  std::unique_ptr<PredicatesIndexCacheMD> predicates_index;

  I_UpdateLoggerPCM *update_logger;

public:
  double measured_time_sd_lookup;

  static constexpr int DEFAULT_WORKER_POOL_SZ = 4;
  static constexpr unsigned long DEFAULT_MAX_QUEUE_SIZE = 10'000'000;

  PredicatesCacheManager(
      std::unique_ptr<PredicatesIndexCacheMD> &&predicates_index);

  PredicatesCacheManager(std::unique_ptr<I_FileRWHandler> &&file_rwhandler);

  PredicatesIndexCacheMD &get_predicates_index_cache();
  void load_all_predicates();

  size_t get_predicate_size(unsigned long predicate_id);
  void remove_key(unsigned long key) override;
  void retrieve_key(unsigned long key) override;

  std::unique_ptr<K2TreeScanner> create_null_k2tree_scanner();

  void merge_add_tree(unsigned long predicate_id, K2TreeMixed &k2tree);

  void merge_delete_tree(unsigned long predicate_id, K2TreeMixed &k2tree);

  void set_update_logger(I_UpdateLoggerPCM *input_update_logger);

  void merge_update(std::vector<K2TreeUpdates> &updates);

  PredicatesCacheManager(const std::string &input_k2tree_filename);

  bool is_loaded_in_memory(unsigned long predicate_id);

private:
  void merge_op_tree(unsigned long predicate_id, K2TreeMixed &to_merge_k2tree,
                     const std::function<void(K2TreeBulkOp &, unsigned long,
                                              unsigned long)> &op,
                     bool create_if_doesnt_exists);
};

#endif // RDFCACHEK2_PREDICATESCACHEMANAGER_HPP
