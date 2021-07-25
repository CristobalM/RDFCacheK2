//
// Created by cristobal on 7/21/21.
//

#include "VDGNode.hpp"
#include <algorithm>
#include <set>
#include <stdexcept>

VDGNode::VDGNode(VarsCollection vars_collection)
    : vars_collection(std::move(vars_collection)) {}

void VDGNode::connect_if_share_vars(VDGNode &other) {

  auto &my_table_vars = vars_collection.get_table_vars();
  auto &my_ref_vars = vars_collection.get_reference_vars();
  auto &other_table_vars = other.vars_collection.get_table_vars();
  auto &other_ref_vars = other.vars_collection.get_reference_vars();
  auto intersection_table_ref_lr =
      intersect_sets(my_table_vars, other_ref_vars);
  auto intersection_table_ref_rl =
      intersect_sets(my_ref_vars, other_table_vars);

  if (!intersection_table_ref_lr.empty() &&
      !intersection_table_ref_rl.empty()) {
    throw std::runtime_error("LR and RL intersections are both non empty");
  }

  if (!intersection_table_ref_lr.empty()) {
    connect_to(other);
    return;
  }
  if (!intersection_table_ref_rl.empty()) {
    other.connect_to(*this);
    return;
  }

  auto table_vars_intersection =
      intersect_sets(my_table_vars, other_table_vars);
  if (!table_vars_intersection.empty()) {
    connect_to(other);
  }
}
std::set<unsigned long>
VDGNode::intersect_sets(std::set<unsigned long> &first,
                        std::set<unsigned long> &second) {
  std::set<unsigned long> result;
  std::set_intersection(first.begin(), first.end(), second.begin(),
                        second.end(), std::inserter(result, result.begin()));
  return result;
}

void VDGNode::connect_to(VDGNode &other) { destinations.push_back(&other); }
int VDGNode::get_position() { return vars_collection.get_position(); }
std::vector<VDGNode *> &VDGNode::get_destinations() { return destinations; }
const VarsCollection &VDGNode::get_collection() { return vars_collection; }
