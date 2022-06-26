//
// Created by Cristobal Miranda, 2020
//

#include "PredicatesCacheManager.hpp"
#include "FileRWHandler.hpp"
#include "k2tree/NullScanner.hpp"
namespace k2cache {

// read/write constructors

PredicatesCacheManager::PredicatesCacheManager(
    std::unique_ptr<PredicatesIndexCacheMD> &&predicates_index,
    std::unique_ptr<UpdatesLogger> &&update_logger)
    : predicates_index(std::move(predicates_index)),
      updates_logger(std::move(update_logger)),
      fully_indexed_cache(*this->predicates_index) {
  this->predicates_index->set_update_logger(this->updates_logger.get());
  updates_logger->recover_all();
}

PredicatesCacheManager::PredicatesCacheManager(
    std::unique_ptr<I_FileRWHandler> &&index_file_handler,
    UpdatesLoggerFilesManager &&updates_logger_fm)
    : PredicatesCacheManager(std::make_unique<PredicatesIndexCacheMD>(
                                 std::move(index_file_handler)),
                             std::make_unique<UpdatesLogger>(
                                 *this, std::move(updates_logger_fm))) {}

PredicatesCacheManager::PredicatesCacheManager(const CacheArgs &cache_args)
    : PredicatesCacheManager(
          std::make_unique<FileRWHandler>(cache_args.index_filename),
          UpdatesLoggerFilesManager(cache_args)) {}

// read only constructors

PredicatesCacheManager::PredicatesCacheManager(
    std::unique_ptr<PredicatesIndexCacheMD> &&predicates_index)
    : predicates_index(std::move(predicates_index)),
      fully_indexed_cache(*predicates_index) {}

PredicatesCacheManager::PredicatesCacheManager(
    std::unique_ptr<I_FileRWHandler> &&index_file_handler)
    : PredicatesCacheManager(std::make_unique<PredicatesIndexCacheMD>(
          std::move(index_file_handler))) {}

PredicatesCacheManager::PredicatesCacheManager(
    const std::string &index_filename)
    : PredicatesCacheManager(std::make_unique<FileRWHandler>(index_filename)) {}

//
// PredicatesCacheManager::PredicatesCacheManager(
//    const std::string &input_k2tree_filename)
//    : PredicatesCacheManager(
//          std::make_unique<FileRWHandler>(input_k2tree_filename)) {}

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
  if (!fetched.exists()) // in the least it should've been added on
                         // predicates_index->add_predicate
    throw std::runtime_error("merge_op_tree: k2tree with predicate_id " +
                             std::to_string(predicate_id) + " not found");
  auto &k2tree_active = fetched.get_mutable();
  K2TreeBulkOp bulk_op(k2tree_active);
  auto points_scanner = to_merge_k2tree.create_full_scanner();
  while (points_scanner->has_next()) {
    auto point = points_scanner->next();
    op(bulk_op, point.first, point.second);
  }
  predicates_index->mark_dirty(predicate_id);
  fully_indexed_cache.resync_predicate(predicate_id);
}
void PredicatesCacheManager::set_update_logger(
    I_UpdateLoggerPCM *input_update_logger) {
  predicates_index->set_update_logger(input_update_logger);
}
void PredicatesCacheManager::merge_update(std::vector<K2TreeUpdates> &updates) {
  for (auto &update : updates) {
    if (!predicates_index->has_predicate_active(update.predicate_id))
      continue;
    if (update.k2tree_add)
      merge_add_tree(update.predicate_id, *update.k2tree_add);
    if (update.k2tree_del)
      merge_delete_tree(update.predicate_id, *update.k2tree_del);
  }
}
FullyIndexedCache &PredicatesCacheManager::get_fully_indexed_cache() {
  return fully_indexed_cache;
}
UpdatesLogger &PredicatesCacheManager::get_updates_logger() {
  if (!updates_logger)
    throw std::runtime_error("There is no updates_logger on read only mode");
  return *updates_logger;
}
} // namespace k2cache