//
// Created by cristobal on 7/21/21.
//

#ifndef RDFCACHEK2_VARDEPENDENCYGRAPH_HPP
#define RDFCACHEK2_VARDEPENDENCYGRAPH_HPP

#include "VDGNode.hpp"
#include "VarsCollection.hpp"
#include <memory>
#include <set>
#include <vector>
class VarDependencyGraph {
  std::vector<std::unique_ptr<VDGNode>> nodes;

  std::vector<int> dds_parents;
  std::vector<int> dds_parents_sz;

  std::vector<int> roots;

public:
  explicit VarDependencyGraph(std::vector<VarsCollection> &&vars_sequence);
  explicit VarDependencyGraph(std::vector<VarsCollection> &vars_collections);

  bool in_same_component(VDGNode &first, VDGNode &second);
  std::vector<std::vector<VDGNode *>> get_connected_components();
  std::vector<std::vector<int>> get_connected_components_positions();

  std::pair<std::vector<std::vector<int>>,
            std::vector<std::vector<std::set<unsigned long>>>>
  get_connected_components_positions_with_sets();

  const std::vector<std::unique_ptr<VDGNode>> &get_nodes();

private:
  // the graph is might not be a DAG, but this algorithm gives us
  // a consistent order in which to perform the queries
  std::vector<VDGNode *> get_topological_sort();

  void topological_sort_dfs(VDGNode &node, std::vector<bool> &visited,
                            std::vector<VDGNode *> &result,
                            int &current_position);

  void build_dds();
  int dds_find(VDGNode &node);
  void dds_union(VDGNode &first, VDGNode &second);
  void find_roots();
  void find_roots_dfs(VDGNode &vdg_node, std::vector<bool> &visited,
                      std::vector<int> &in_degree);
};

#endif // RDFCACHEK2_VARDEPENDENCYGRAPH_HPP
