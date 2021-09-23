//
// Created by cristobal on 21-09-21.
//

#ifndef RDFCACHEK2_TRIPLENODEID_HPP
#define RDFCACHEK2_TRIPLENODEID_HPP

#include "NodeId.hpp"

struct TripleNodeId {
  NodeId subject;
  NodeId predicate;
  NodeId object;
  TripleNodeId(NodeId subject, NodeId predicate, NodeId object);
};

#endif // RDFCACHEK2_TRIPLENODEID_HPP
