//
// Created by cristobal on 9/5/21.
//

#ifndef RDFCACHEK2_UPDATERSESSION_HPP
#define RDFCACHEK2_UPDATERSESSION_HPP

#include "I_Updater.hpp"
#include "TaskProcessor.hpp"
#include <Cache.hpp>
#include <K2TreeBulkOp.hpp>
class UpdaterSession : public I_Updater {
  int update_id;
  TaskProcessor *task_processor;
  Cache *cache;

  using tree_update_pair_t =
      std::pair<std::unique_ptr<K2TreeMixed>, std::unique_ptr<K2TreeBulkOp>>;

  std::unordered_map<unsigned long, tree_update_pair_t> added_triples;
  std::unordered_map<unsigned long, tree_update_pair_t> removed_triples;

public:
  UpdaterSession(int update_id, TaskProcessor *task_processor, Cache *cache);

  void add_triple(RDFTripleResource &rdf_triple_resource) override;
  void delete_triple(RDFTripleResource &rdf_triple_resource) override;
  K2TreeBulkOp &get_tree_inserter(RDFTripleResource &triple_resource);
  K2TreeConfig get_config();
  K2TreeBulkOp &get_tree_bulk_op(
      std::unordered_map<unsigned long, tree_update_pair_t> &map_src,
      RDFTripleResource &triple_resource);
};

#endif // RDFCACHEK2_UPDATERSESSION_HPP
