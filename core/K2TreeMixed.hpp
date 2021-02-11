#ifndef RDFCACHEK2_K2TREE_MIXED_HPP
#define RDFCACHEK2_K2TREE_MIXED_HPP

#include <istream>
#include <memory>
#include <utility>
#include <vector>

#include "ResultTable.hpp"
#include "k2tree_stats.hpp"

#include <serialization_util.hpp>

extern "C" {
#include <definitions.h>
#include <k2node.h>
}

struct k2node;
struct k2qstate;

struct K2TreeConfig {
  uint32_t treedepth;
  uint32_t max_node_count;
  uint32_t cut_depth;

  void write_to_ostream(std::ostream &os) {
    write_u32(os, treedepth);
    write_u32(os, max_node_count);
    write_u32(os, cut_depth);
  }

  void read_from_istream(std::istream &is) {
    treedepth = read_u32(is);
    max_node_count = read_u32(is);
    cut_depth = read_u32(is);
  }
};

class K2TreeMixed {
  struct k2node *root;
  std::unique_ptr<struct k2qstate> st;

public:
  explicit K2TreeMixed(uint32_t treedepth);
  K2TreeMixed(uint32_t treedepth, uint32_t max_node_count);
  K2TreeMixed(uint32_t treedepth, uint32_t max_node_count, uint32_t cut_depth);
  K2TreeMixed(struct k2node *root, uint32_t treedepth, uint32_t max_node_count,
              uint32_t cut_depth);

  K2TreeMixed(K2TreeConfig config);

  K2TreeMixed(const K2TreeMixed &other) = delete;
  K2TreeMixed &operator=(const K2TreeMixed &other) = delete;

  K2TreeMixed(K2TreeMixed &&other);
  K2TreeMixed &operator=(K2TreeMixed &&other);

  ~K2TreeMixed();

  void insert(unsigned long col, unsigned long row);
  bool has(unsigned long col, unsigned long row);

  static std::vector<unsigned long>
  sip_join_k2trees(const std::vector<K2TreeMixed *> &trees,
                   std::vector<struct sip_ipoint> &join_coordinates);

  unsigned long get_tree_depth();

  std::vector<std::pair<unsigned long, unsigned long>> get_all_points();
  std::vector<unsigned long> get_row(unsigned long row);
  std::vector<unsigned long> get_column(unsigned long col);

  void scan_points(point_reporter_fun_t fun_reporter, void *report_state);
  void traverse_row(unsigned long row, point_reporter_fun_t fun_reporter,
                    void *report_state);
  void traverse_column(unsigned long column, point_reporter_fun_t fun_reporter,
                       void *report_state);

  struct k2tree_measurement measure_in_memory_size();

  ResultTable column_as_table(unsigned long column);
  ResultTable row_as_table(unsigned long row);

  K2TreeStats k2tree_stats();

  bool same_as(const K2TreeMixed &other);

  void write_to_ostream(std::ostream &os);
  static K2TreeMixed read_from_istream(std::istream &is);

private:
  void clean_up();
};

#endif
