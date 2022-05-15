//
// Created by Cristobal Miranda, 2020
//

#include "PredicatesCacheManager.hpp"
#include "K2TreeBulkOp.hpp"
#include "NullScanner.hpp"
#include <FileRWHandler.hpp>
#include <chrono>
#include <filesystem>
#include <functional>

namespace fs = std::filesystem;

PredicatesCacheManager::PredicatesCacheManager(
    std::unique_ptr<PredicatesIndexCacheMD> &&predicates_index)
    : predicates_index(std::move(predicates_index)),
      measured_time_sd_lookup(0) {}

PredicatesCacheManager::PredicatesCacheManager(
    std::unique_ptr<I_FileRWHandler> &&file_rwhandler)
    : PredicatesCacheManager(
          std::make_unique<PredicatesIndexCacheMD>(std::move(file_rwhandler))) {
}

PredicatesCacheManager::PredicatesCacheManager(
    const std::string &input_k2tree_filename)
    : PredicatesCacheManager(
          std::make_unique<FileRWHandler>(input_k2tree_filename)) {}

PredicatesIndexCacheMD &PredicatesCacheManager::get_predicates_index_cache() {
  return *predicates_index;
}

void PredicatesCacheManager::load_all_predicates() {
  predicates_index->load_all_predicates();
}

size_t PredicatesCacheManager::get_predicate_size(unsigned long predicate_id) {
  auto &metadata_map = predicates_index->get_metadata().get_map();
  if (metadata_map.find(predicate_id) == metadata_map.end())
    return 0;
  return metadata_map.at(predicate_id).tree_size_in_memory;
}

void PredicatesCacheManager::remove_key(unsigned long key) {
  predicates_index->discard_in_memory_predicate(key);
}
void PredicatesCacheManager::retrieve_key(unsigned long key) {
  predicates_index->load_single_predicate(key);
}
std::unique_ptr<K2TreeScanner>
PredicatesCacheManager::create_null_k2tree_scanner() {
  return std::make_unique<NullScanner>();
}
void PredicatesCacheManager::merge_add_tree(unsigned long predicate_id,
                                            K2TreeMixed &k2tree) {
  merge_op_tree(
      predicate_id, k2tree,
      [](K2TreeBulkOp &bulk_op, unsigned long col, unsigned long row) {
        bulk_op.insert(col, row);
      },
      true);
}
void PredicatesCacheManager::merge_delete_tree(unsigned long predicate_id,
                                               K2TreeMixed &k2tree) {
  merge_op_tree(
      predicate_id, k2tree,
      [](K2TreeBulkOp &bulk_op, unsigned long col, unsigned long row) {
        bulk_op.remove(col, row);
      },
      false);
}
void PredicatesCacheManager::merge_op_tree(
    unsigned long predicate_id, K2TreeMixed &to_merge_k2tree,
    const std::function<void(K2TreeBulkOp &, unsigned long, unsigned long)> &op,
    bool create_if_doesnt_exists) {
  if (!predicates_index->has_predicate_active(predicate_id)) {
    if (!create_if_doesnt_exists)
      return;
    predicates_index->add_predicate(predicate_id);
  }
  auto fetched = predicates_index->fetch_k2tree(predicate_id);
  auto &k2tree_active = fetched.get_mutable();
  K2TreeBulkOp bulk_op(k2tree_active);
  auto points_scanner = to_merge_k2tree.create_full_scanner();
  while (points_scanner->has_next()) {
    auto point = points_scanner->next();
    op(bulk_op, point.first, point.second);
  }
  predicates_index->mark_dirty(predicate_id);
}
void PredicatesCacheManager::set_update_logger(
    I_UpdateLoggerPCM *input_update_logger) {
  predicates_index->set_update_logger(input_update_logger);
}
void PredicatesCacheManager::merge_update(std::vector<K2TreeUpdates> &updates) {
  for (auto &update : updates) {
    if (update.k2tree_add)
      merge_add_tree(update.predicate_id, *update.k2tree_add);
    if (update.k2tree_del)
      merge_delete_tree(update.predicate_id, *update.k2tree_del);
  }
}