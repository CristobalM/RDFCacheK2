//
// Created by cristobal on 21-09-21.
//

#include "TripleNodeId.hpp"
#include "NodeId.hpp"
namespace k2cache {
TripleNodeId::TripleNodeId(NodeId subject, NodeId predicate, NodeId object)
    : subject(subject), predicate(predicate), object(object) {}
} // namespace k2cache