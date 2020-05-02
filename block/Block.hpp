//
// Created by Cristobal Miranda, 2020
//

#ifndef RDFCACHEK2_BLOCK_HPP
#define RDFCACHEK2_BLOCK_HPP


extern "C" {
#include <block.h>
#include <block_topology.h>
#include <block_frontier.h>
#include <queries_state.h>
}

#include <stdexcept>
#include <string>
#include <vector>

class Block {
  struct block *b;
  struct queries_state qs;

public:

  explicit Block(uint32_t tree_depth);
  Block(uint32_t tree_depth, uint32_t max_node_count);

  ~Block() noexcept(false);

  void insert(unsigned long col, unsigned long row);
  bool has(unsigned long col, unsigned long row);
  std::vector<std::pair<unsigned long, unsigned long>> scan_points();
};

#endif //RDFCACHEK2_BLOCK_HPP
