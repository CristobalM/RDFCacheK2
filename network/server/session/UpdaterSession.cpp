//
// Created by cristobal on 9/5/21.
//

#include "UpdaterSession.hpp"
#include "updating/K2TreeUpdates.hpp"

namespace k2cache {
UpdaterSession::UpdaterSession(TaskProcessor *task_processor,
                               CacheContainer *cache)
    : task_processor(task_processor), cache(cache),
      updater_k2tree_config(get_initial_update_k2tree_config()) {}

void UpdaterSession::commit_updates() {
  auto write_lock = task_processor->acquire_write_lock();
  do_commit_updates();
  (void)(write_lock);
}

void UpdaterSession::add_triple(TripleNodeId &rdf_triple_resource) {
  auto &tree_inserter = get_tree_inserter(rdf_triple_resource);
  auto subject_id = rdf_triple_resource.subject.get_value();
  auto object_id = rdf_triple_resource.object.get_value();
  auto mapped_subject = cache->get_nodes_ids_manager().get_id_or_create((long)subject_id);
  auto mapped_object = cache->get_nodes_ids_manager().get_id_or_create((long)object_id);
  tree_inserter.insert(mapped_subject, mapped_object);
}
void UpdaterSession::delete_triple(TripleNodeId &rdf_triple_resource) {
  auto predicate_id = rdf_triple_resource.predicate.get_value();
  if (predicate_id == 0)
    return;
  auto subject_id = rdf_triple_resource.subject.get_value();
  if (subject_id == 0)
    return;
  auto object_id = rdf_triple_resource.subject.get_value();
  if (object_id == 0)
    return;

  auto mapped_subject = cache->get_nodes_ids_manager().get_id_or_create((long)subject_id);
  auto mapped_predicate = cache->get_nodes_ids_manager().get_id_or_create((long)predicate_id);
  auto mapped_object = cache->get_nodes_ids_manager().get_id_or_create((long)object_id);

  auto &tree_deleter = get_tree_deleter(mapped_predicate);
  tree_deleter.insert(mapped_subject, mapped_object);
}

K2TreeBulkOp &UpdaterSession::get_tree_inserter(TripleNodeId &triple_resource) {
  return get_tree_bulk_op(added_triples, triple_resource);
}
K2TreeConfig UpdaterSession::get_config() {
  //  return cache->get_pcm().get_predicates_index_cache().get_config();
  return updater_k2tree_config;
}

K2TreeBulkOp &UpdaterSession::get_tree_bulk_op(tmap_t &map_src,
                                               TripleNodeId &triple_resource) {
  unsigned long predicate_id = triple_resource.predicate.get_value();
  auto mapped_predicate = cache->get_nodes_ids_manager().get_id_or_create((long)predicate_id);
  return get_tree_bulk_op_id(map_src, mapped_predicate);
}

K2TreeBulkOp &UpdaterSession::get_tree_deleter(unsigned long id) {
  return get_tree_bulk_op_id(removed_triples, id);
}

K2TreeBulkOp &UpdaterSession::get_tree_bulk_op_id(tmap_t &map_src,
                                                  unsigned long predicate_id) {
  auto it = map_src.find(predicate_id);
  if (it == map_src.end()) {
    auto config = get_config();
    auto k2tree = std::make_unique<K2TreeMixed>(config);
    auto bulk_op = std::make_unique<K2TreeBulkOp>(*k2tree);
    auto *bulk_op_ptr = bulk_op.get();
    map_src[predicate_id] = {std::move(k2tree), std::move(bulk_op)};
    return *bulk_op_ptr;
  }
  return *it->second.second;
}
void UpdaterSession::log_updates() {
  std::vector<K2TreeUpdates> k2tree_updates;

  auto updates_size = std::max(added_triples.size(), removed_triples.size());
  k2tree_updates.reserve(updates_size);
  std::unordered_set<unsigned long> visited_del;
  visited_del.reserve(removed_triples.size() * 2);
  for (auto &kv : added_triples) {
    unsigned long predicate_id = kv.first;
    K2TreeMixed *add_tree = kv.second.first.get();
    K2TreeMixed *del_tree = nullptr;
    auto it_removed = removed_triples.find(predicate_id);
    if (it_removed != removed_triples.end()) {
      del_tree = it_removed->second.first.get();
      visited_del.insert(predicate_id);
    }
    k2tree_updates.emplace_back(predicate_id, add_tree, del_tree);
  }
  for (auto &kv : removed_triples) {
    unsigned long predicate_id = kv.first;
    if (visited_del.find(predicate_id) != visited_del.end())
      continue;
    K2TreeMixed *del_tree = kv.second.first.get();
    K2TreeMixed *add_tree = nullptr;
    k2tree_updates.emplace_back(predicate_id, add_tree, del_tree);
  }

  task_processor->log_updates(k2tree_updates);
}
void UpdaterSession::do_commit_updates() { log_updates(); }
K2TreeConfig UpdaterSession::get_initial_update_k2tree_config() {
  K2TreeConfig config{};
  config.cut_depth = 0;
  config.max_node_count = 1024;
  config.treedepth = 32;
  return config;
}

} // namespace k2cache