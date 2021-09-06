//
// Created by cristobal on 9/5/21.
//

#include "UpdaterSession.hpp"
UpdaterSession::UpdaterSession(int update_id, TaskProcessor *task_processor,
                               Cache *cache)
    : update_id(update_id), task_processor(task_processor), cache(cache) {}
void UpdaterSession::add_triple(RDFTripleResource &rdf_triple_resource) {

  auto &tree_inserter = get_tree_inserter(rdf_triple_resource);
}
void UpdaterSession::delete_triple(RDFTripleResource &rdf_triple_resource) {}
K2TreeBulkOp &
UpdaterSession::get_tree_inserter(RDFTripleResource &triple_resource) {
  return get_tree_bulk_op(added_triples, triple_resource);
}
K2TreeConfig UpdaterSession::get_config() {
  return cache->get_pcm().get_predicates_index_cache().get_config();
}

K2TreeBulkOp &UpdaterSession::get_tree_bulk_op(
    std::unordered_map<unsigned long, tree_update_pair_t> &map_src,
    RDFTripleResource &triple_resource) {
  unsigned long predicate_id =
      cache->get_pcm().get_resource_index(triple_resource.predicate);
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
