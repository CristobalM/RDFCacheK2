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

#include <mutex>

#include "ResultTable.hpp"
#include "k2tree_stats.hpp"

class K2Tree {
  struct block *root;
  std::unique_ptr<struct queries_state> qs;

public:
  explicit K2Tree(uint32_t tree_depth);
  K2Tree(uint32_t tree_depth, uint32_t max_node_count);
  K2Tree(struct block *root, uint32_t tree_depth, uint32_t nodes_in_block);

  K2Tree(const K2Tree &other) = delete;
  K2Tree &operator=(K2Tree &rhs) = delete;

  K2Tree(K2Tree &&other);
  K2Tree &operator=(K2Tree &&rhs);

  ~K2Tree() noexcept(false);

  void insert(unsigned long col, unsigned long row);
  bool has(unsigned long col, unsigned long row);

  unsigned long get_tree_depth();

  std::vector<std::pair<unsigned long, unsigned long>> get_all_points();
  std::vector<unsigned long> get_row(unsigned long row);
  std::vector<unsigned long> get_column(unsigned long row);

  void scan_points(point_reporter_fun_t fun_reporter, void *report_state);
  void traverse_row(unsigned long row, point_reporter_fun_t fun_reporter,
                    void *report_state);
  void traverse_column(unsigned long column, point_reporter_fun_t fun_reporter,
                       void *report_state);

  struct k2tree_measurement measure_in_memory_size();

  ResultTable column_as_table(unsigned long column);
  ResultTable row_as_table(unsigned long row);

  K2TreeStats k2tree_stats();

  bool same_as(const K2Tree &other);

  void write_to_ostream(std::ostream &os);
  static K2Tree read_from_istream(std::istream &is);
};

#endif // RDFCACHEK2_K2TREE_HPP
