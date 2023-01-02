//
// Created by cristobal on 9/5/21.
//

#ifndef RDFCACHEK2_UPDATERSESSION_HPP
#define RDFCACHEK2_UPDATERSESSION_HPP

#include "CacheContainer.hpp"
#include "Updater.hpp"
#include "k2tree/K2TreeBulkOp.hpp"
#include "nodeids/TripleNodeId.hpp"
#include "server/tasks/TaskProcessor.hpp"
#include <unordered_map>
namespace k2cache {

class UpdaterSession : public Updater {
  TaskProcessor *task_processor;
  CacheContainer *cache;

  using tree_update_pair_t =
      std::pair<std::unique_ptr<K2TreeMixed>, std::unique_ptr<K2TreeBulkOp>>;

  using tmap_t = std::map<uint64_t, tree_update_pair_t>;

  tmap_t added_triples;
  tmap_t removed_triples;

  K2TreeConfig updater_k2tree_config;

public:
  UpdaterSession(TaskProcessor *task_processor, CacheContainer *cache);

  void add_triple(TripleNodeId &rdf_triple_resource) override;
  void delete_triple(TripleNodeId &rdf_triple_resource) override;
  void commit_updates() override;
  static K2TreeConfig get_initial_update_k2tree_config();

private:
  K2TreeBulkOp &get_tree_inserter(TripleNodeId &triple_resource);
  K2TreeConfig get_config();
  K2TreeBulkOp &get_tree_bulk_op(tmap_t &map_src,
                                 TripleNodeId &triple_resource);
  K2TreeBulkOp &get_tree_deleter(uint64_t id);
  K2TreeBulkOp &get_tree_bulk_op_id(tmap_t &map_src,
                                    uint64_t predicate_id);
  void log_updates();
  void do_commit_updates();
};
} // namespace k2cache
#endif // RDFCACHEK2_UPDATERSESSION_HPP
