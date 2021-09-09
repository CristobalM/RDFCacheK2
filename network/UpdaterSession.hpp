//
// Created by cristobal on 9/5/21.
//

#ifndef RDFCACHEK2_UPDATERSESSION_HPP
#define RDFCACHEK2_UPDATERSESSION_HPP

#include "I_Updater.hpp"
#include "TaskProcessor.hpp"
#include <Cache.hpp>
#include <K2TreeBulkOp.hpp>
#include <unordered_map>
class UpdaterSession : public I_Updater {
  TaskProcessor *task_processor;
  Cache *cache;

  using tree_update_pair_t =
      std::pair<std::unique_ptr<K2TreeMixed>, std::unique_ptr<K2TreeBulkOp>>;

  using tmap_t = std::map<unsigned long, tree_update_pair_t>;

  tmap_t added_triples;
  tmap_t removed_triples;

  const unsigned long last_id_known;
  std::unique_ptr<NaiveDynamicStringDictionary> added_resources;

public:
  UpdaterSession(TaskProcessor *task_processor, Cache *cache);

  void add_triple(RDFTripleResource &rdf_triple_resource) override;
  void delete_triple(RDFTripleResource &rdf_triple_resource) override;
  void commit_updates() override;

private:
  K2TreeBulkOp &get_tree_inserter(RDFTripleResource &triple_resource);
  K2TreeConfig get_config();
  K2TreeBulkOp &get_tree_bulk_op(tmap_t &map_src,
                                 RDFTripleResource &triple_resource);
  unsigned long add_resource_get_id(RDFResource &resource);
  inline unsigned long get_resource_id(RDFResource &resource);
  unsigned long get_or_create_resource_id(RDFResource &resource);
  K2TreeBulkOp &get_tree_deleter(unsigned long id);
  K2TreeBulkOp &get_tree_bulk_op_id(tmap_t &map_src,
                                    unsigned long predicate_id);
  void log_updates();
  void do_commit_updates();
};

#endif // RDFCACHEK2_UPDATERSESSION_HPP
