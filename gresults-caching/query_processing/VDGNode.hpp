//
// Created by cristobal on 7/21/21.
//

#ifndef RDFCACHEK2_VDGNODE_HPP
#define RDFCACHEK2_VDGNODE_HPP

#include "VarsCollection.hpp"
#include <memory>
#include <vector>
class VDGNode {
  VarsCollection vars_collection;
  std::vector<VDGNode *> destinations;

public:
  explicit VDGNode(VarsCollection vars_collection);

  void connect_if_share_vars(VDGNode &other);
  static std::set<unsigned long>
  intersect_sets(std::set<unsigned long> &first,
                 std::set<unsigned long> &second);
  void connect_to(VDGNode &other);
  int get_position();
  std::vector<VDGNode *> &get_destinations();
  const VarsCollection &get_collection();
};

#endif // RDFCACHEK2_VDGNODE_HPP
