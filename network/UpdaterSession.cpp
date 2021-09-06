//
// Created by cristobal on 9/5/21.
//

#include "UpdaterSession.hpp"
#include "K2TreeUpdates.hpp"
UpdaterSession::UpdaterSession(TaskProcessor *task_processor,
                               Cache *cache)
    : task_processor(task_processor), cache(cache),
      last_id_known(cache->get_pcm().get_last_id()) {}

void UpdaterSession::commit_updates() {
  auto write_lock = task_processor->acquire_write_lock();
  do_commit_updates();
  (void)(write_lock);
}

void UpdaterSession::add_triple(RDFTripleResource &rdf_triple_resource) {

  auto &tree_inserter = get_tree_inserter(rdf_triple_resource);
  auto subject_id = get_or_create_resource_id(rdf_triple_resource.subject);
  auto object_id = get_or_create_resource_id(rdf_triple_resource.object);
  tree_inserter.insert(subject_id, object_id);
}
void UpdaterSession::delete_triple(RDFTripleResource &rdf_triple_resource) {
  auto predicate_id = get_resource_id(rdf_triple_resource.predicate);
  if (predicate_id == 0)
    return;
  auto subject_id = get_resource_id(rdf_triple_resource.subject);
  if (subject_id == 0)
    return;
  auto object_id = get_resource_id(rdf_triple_resource.subject);
  if (object_id == 0)
    return;

  auto &tree_deleter = get_tree_deleter(predicate_id);
  tree_deleter.insert(subject_id, object_id);
}

K2TreeBulkOp &
UpdaterSession::get_tree_inserter(RDFTripleResource &triple_resource) {
  return get_tree_bulk_op(added_triples, triple_resource);
}
K2TreeConfig UpdaterSession::get_config() {
  return cache->get_pcm().get_predicates_index_cache().get_config();
}

K2TreeBulkOp &
UpdaterSession::get_tree_bulk_op(tmap_t &map_src,
                                 RDFTripleResource &triple_resource) {
  unsigned long predicate_id =
      get_or_create_resource_id(triple_resource.predicate);
  return get_tree_bulk_op_id(map_src, predicate_id);
}
unsigned long UpdaterSession::add_resource_get_id(RDFResource &resource) {
  if (!added_resources) {
    added_resources = std::make_unique<NaiveDynamicStringDictionary>();
  }
  auto id = added_resources->locate_resource(resource);
  if (id != 0)
    return id;

  added_resources->add_resource(resource);
  return added_resources->locate_resource(resource) + last_id_known;
}
unsigned long UpdaterSession::get_resource_id(RDFResource &resource) {
  return cache->get_pcm().get_resource_index(resource);
}
unsigned long UpdaterSession::get_or_create_resource_id(RDFResource &resource) {
  auto id = get_resource_id(resource);
  if (id != 0)
    return id;
  return add_resource_get_id(resource);
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

  task_processor->log_updates(added_resources.get(), k2tree_updates);
}
void UpdaterSession::do_commit_updates() {
  log_updates();
  if (added_resources)
    cache->get_pcm().merge_with_extra_dict(*added_resources);
  for (auto &kv : added_triples) {
    cache->get_pcm().merge_add_tree(kv.first, *kv.second.first);
  }
  for (auto &kv : removed_triples) {
    cache->get_pcm().merge_delete_tree(kv.first, *kv.second.first);
  }
}
