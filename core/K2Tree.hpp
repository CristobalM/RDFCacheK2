//
// Created by Cristobal Miranda, 2020
//

#ifndef RDFCACHEK2_K2TREE_HPP
#define RDFCACHEK2_K2TREE_HPP

extern "C" {
#include <block.h>
#include <block_frontier.h>
#include <block_topology.h>
#include <queries_state.h>
}

#include <memory>
#include <sstream>
#include <stdexcept>
#include <string>
#include <vector>

#include <graph_result.pb.h>

struct K2TreeStats {
  int allocated_u32s;
  int nodes_count;
  int containers_sz_sum;
};

class K2Tree {
  struct block *root;
  struct queries_state qs;

public:
  explicit K2Tree(uint32_t tree_depth);
  K2Tree(uint32_t tree_depth, uint32_t max_node_count);
  explicit K2Tree(struct block *root);

  ~K2Tree() noexcept(false);

  void insert(unsigned long col, unsigned long row);
  bool has(unsigned long col, unsigned long row);
  std::vector<std::pair<unsigned long, unsigned long>> scan_points();

  void serialize_tree(std::stringstream &stream);

  static std::unique_ptr<K2Tree>
  from_binary_stream(std::istringstream &k2tree_binary_stream);

  K2TreeStats k2tree_stats();

  void produce_proto(proto_msg::K2Tree *to_feed);

  K2Tree(proto_msg::K2Tree &k2tree_proto);
};

#endif // RDFCACHEK2_K2TREE_HPP
