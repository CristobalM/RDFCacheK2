//
// Created by cristobal on 26-06-22.
//

#ifndef RDFCACHEK2_NODEIDSMANAGER_HPP
#define RDFCACHEK2_NODEIDSMANAGER_HPP

namespace k2cache {
class NodesSequence;
struct NodeIdsManager {
  virtual ~NodeIdsManager() = default;
  virtual NodesSequence &get_nodes_sequence() = 0;
  virtual long get_id(long real_id) = 0;
  virtual long get_real_id(long mapped_id) = 0;
};
} // namespace k2cache

#endif // RDFCACHEK2_NODEIDSMANAGER_HPP
