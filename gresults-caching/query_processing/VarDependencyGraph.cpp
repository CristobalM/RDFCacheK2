//
// Created by cristobal on 7/21/21.
//

#include "VarDependencyGraph.hpp"
#include "QueryProcHashing.hpp"
#include <algorithm>
#include <map>

VarDependencyGraph::VarDependencyGraph(
    std::vector<VarsCollection> &&vars_sequence)
    : VarDependencyGraph(vars_sequence) {}

VarDependencyGraph::VarDependencyGraph(
    std::vector<VarsCollection> &vars_collections) {
  nodes.reserve(vars_collections.size());
  for (auto &var_collection : vars_collections) {
    nodes.push_back(std::make_unique<VDGNode>(var_collection));
  }

  for (size_t i = 0; i < nodes.size(); i++) {
    for (size_t j = i + 1; j < nodes.size(); j++) {
      nodes[i]->connect_if_share_vars(*nodes[j]);
    }
  }
  build_dds();
  find_roots();
}

std::vector<VDGNode *> VarDependencyGraph::get_topological_sort() {
  std::vector<bool> visited(nodes.size(), false);
  std::vector<VDGNode *> result(nodes.size(), nullptr);
  int current_position = 0;
  /*
  for (auto &node : nodes) {
    topological_sort_dfs(*node, visited, result, current_position);
  }
   */
  std::map<int, std::vector<int>> roots_grouped_by_component;

  for (int i = 0; i < (int)roots.size(); i++) {
    auto &curr_node = *nodes[roots[i]];
    int cc_id = dds_find(curr_node);
    roots_grouped_by_component[cc_id].push_back(roots[i]);
  }

  for (auto &kv_rgc : roots_grouped_by_component) {
    auto &rgc = kv_rgc.second;
    std::sort(rgc.begin(), rgc.end(), [this](int lhs_ri, int rhs_ri) {
      return nodes[lhs_ri]->get_position() < nodes[rhs_ri]->get_position();
    });
    for (auto r_i : rgc) {
      auto &node = *nodes[r_i];
      topological_sort_dfs(node, visited, result, current_position);
    }
  }

  return result;
}

void VarDependencyGraph::topological_sort_dfs(VDGNode &node,
                                              std::vector<bool> &visited,
                                              std::vector<VDGNode *> &result,
                                              int &current_position) {
  if (visited[node.get_position()])
    return;
  visited[node.get_position()] = true;
  result[current_position++] = &node;
  for (auto *dst : node.get_destinations()) {
    topological_sort_dfs(*dst, visited, result, current_position);
  }
}
void VarDependencyGraph::build_dds() {
  dds_parents = std::vector<int>(nodes.size(), -1);
  dds_parents_sz = std::vector<int>(nodes.size(), 1);
  for (auto &node : nodes) {
    dds_parents[node->get_position()] = node->get_position();
  }
  for (auto &node : nodes) {
    for (auto *other_node : node->get_destinations()) {
      if (dds_find(*node) != dds_find(*other_node)) {
        dds_union(*node, *other_node);
      }
    }
  }
}
int VarDependencyGraph::dds_find(VDGNode &node) {
  auto &parent = dds_parents[node.get_position()];
  while (parent != dds_parents[parent]) {
    parent = dds_parents[parent];
  }
  return parent;
}

void VarDependencyGraph::dds_union(VDGNode &first, VDGNode &second) {
  auto first_parent = dds_find(first);
  auto second_parent = dds_find(second);
  if (first_parent == second_parent)
    return;

  if (dds_parents_sz[first_parent] > dds_parents_sz[second_parent]) {
    dds_parents[second_parent] = first_parent;
    dds_parents_sz[first_parent] += dds_parents_sz[second_parent];
  } else {
    dds_parents[first_parent] = second_parent;
    dds_parents_sz[second_parent] += dds_parents_sz[first_parent];
  }
}
bool VarDependencyGraph::in_same_component(VDGNode &first, VDGNode &second) {
  return dds_find(first) == dds_find(second);
}

std::vector<std::vector<VDGNode *>>
VarDependencyGraph::get_connected_components() {
  std::vector<std::vector<VDGNode *>> result;

  auto topological_sort = get_topological_sort();

  auto *first_node = topological_sort[0];

  std::vector<VDGNode *> current_cc;
  current_cc.push_back(first_node);
  for (size_t i = 1; i < topological_sort.size(); i++) {
    auto *current_node = topological_sort[i];
    if (in_same_component(*first_node, *current_node)) {
      current_cc.push_back(current_node);
    } else {
      result.push_back(std::move(current_cc));
      current_cc = std::vector<VDGNode *>();
      first_node = current_node;
    }
  }
  if (!current_cc.empty()) {
    result.push_back(std::move(current_cc));
  }
  return result;
}
std::vector<std::vector<int>>
VarDependencyGraph::get_connected_components_positions() {
  auto ccs = get_connected_components();
  std::vector<std::vector<int>> result;
  for (auto &cc : ccs) {
    std::vector<int> cc_ps;
    cc_ps.reserve(cc.size());
    for (auto *vdg_node : cc) {
      cc_ps.push_back(vdg_node->get_position());
    }
    result.push_back(std::move(cc_ps));
  }
  return result;
}
const std::vector<std::unique_ptr<VDGNode>> &VarDependencyGraph::get_nodes() {
  return nodes;
}
std::pair<std::vector<std::vector<int>>,
          std::vector<std::vector<std::set<unsigned long>>>>
VarDependencyGraph::get_connected_components_positions_with_sets() {
  auto ccs = get_connected_components();
  std::vector<std::vector<int>> positions;
  for (auto &cc : ccs) {
    std::vector<int> cc_ps;
    cc_ps.reserve(cc.size());
    for (auto *vdg_node : cc) {
      cc_ps.push_back(vdg_node->get_position());
    }
    positions.push_back(std::move(cc_ps));
  }

  std::vector<std::vector<std::set<unsigned long>>> cc_sets;

  for (auto &cc : ccs) {
    std::vector<std::set<unsigned long>> sets;
    for (auto *vdg_node : cc) {
      const auto &table_vars = vdg_node->get_collection().get_table_vars_c();
      sets.push_back(table_vars);
    }
    cc_sets.push_back(sets);
  }

  return {positions, cc_sets};
}

void VarDependencyGraph::find_roots() {
  std::vector<bool> visited(nodes.size(), false);
  std::vector<int> in_degree(nodes.size(), 0);
  for (auto &node : nodes) {
    find_roots_dfs(*node, visited, in_degree);
  }
  for (size_t i = 0; i < nodes.size(); i++) {
    if (in_degree[i] == 0) {
      roots.push_back(static_cast<int>(i));
    }
  }
}
void VarDependencyGraph::find_roots_dfs(VDGNode &vdg_node,
                                        std::vector<bool> &visited,
                                        std::vector<int> &in_degree) {
  if (visited[vdg_node.get_position()])
    return;
  visited[vdg_node.get_position()] = true;
  for (auto *dst_node : vdg_node.get_destinations()) {
    in_degree[dst_node->get_position()]++;
  }
  for (auto *dst_node : vdg_node.get_destinations()) {
    find_roots_dfs(*dst_node, visited, in_degree);
  }
}
