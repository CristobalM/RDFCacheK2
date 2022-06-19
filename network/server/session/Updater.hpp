//
// Created by cristobal on 9/5/21.
//

#ifndef RDFCACHEK2_UPDATER_HPP
#define RDFCACHEK2_UPDATER_HPP

#include "nodeids/NodeId.hpp"
#include "nodeids/TripleNodeId.hpp"
class Updater {
public:
  virtual ~Updater() = default;

  virtual void add_triple(TripleNodeId &rdf_triple_resource) = 0;
  virtual void delete_triple(TripleNodeId &rdf_triple_resource) = 0;
  void add_triple(TripleNodeId &&rdf_triple_resource) {
    return add_triple(rdf_triple_resource);
  }
  void delete_triple(TripleNodeId &&rdf_triple_resource) {
    return delete_triple(rdf_triple_resource);
  }
  virtual void commit_updates() = 0;
};

#endif // RDFCACHEK2_UPDATER_HPP
