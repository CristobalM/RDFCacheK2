//
// Created by cristobal on 9/5/21.
//

#ifndef RDFCACHEK2_I_UPDATER_HPP
#define RDFCACHEK2_I_UPDATER_HPP

#include "RDFTriple.hpp"
class I_Updater {
public:
  virtual ~I_Updater() = default;

  virtual void add_triple(RDFTripleResource &rdf_triple_resource) = 0;
  virtual void delete_triple(RDFTripleResource &rdf_triple_resource) = 0;
  void add_triple(RDFTripleResource &&rdf_triple_resource){
    return add_triple(rdf_triple_resource);
  }
  void delete_triple(RDFTripleResource &&rdf_triple_resource){
    return delete_triple(rdf_triple_resource);
  }
  virtual void commit_updates() = 0;
};

#endif // RDFCACHEK2_I_UPDATER_HPP
