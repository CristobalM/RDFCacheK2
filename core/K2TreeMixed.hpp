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
  uint64_t points_count;

public:
  explicit K2TreeMixed(uint32_t treedepth);
  K2TreeMixed(uint32_t treedepth, uint32_t max_node_count);
  K2TreeMixed(uint32_t treedepth, uint32_t max_node_count, uint32_t cut_depth);
  K2TreeMixed(struct k2node *root, uint32_t treedepth, uint32_t max_node_count,
              uint32_t cut_depth, uint64_t points_count);

  K2TreeMixed(K2TreeConfig config);

  K2TreeMixed(const K2TreeMixed &other) = delete;
  K2TreeMixed &operator=(const K2TreeMixed &other) = delete;

  K2TreeMixed(K2TreeMixed &&other);
  K2TreeMixed &operator=(K2TreeMixed &&other);

  ~K2TreeMixed();

  struct k2node *get_root_k2node();
  struct k2qstate *get_k2qstate();

  void insert(unsigned long col, unsigned long row);
  bool has(unsigned long col, unsigned long row) const;

  static std::vector<unsigned long>
  sip_join_k2trees(const std::vector<const K2TreeMixed *> &trees,
                   std::vector<struct sip_ipoint> &join_coordinates);

  unsigned long get_tree_depth();

  std::vector<std::pair<unsigned long, unsigned long>> get_all_points();
  std::vector<unsigned long> get_row(unsigned long row) const;
  std::vector<unsigned long> get_column(unsigned long col) const;

  void scan_points(point_reporter_fun_t fun_reporter, void *report_state) const;
  std::vector<std::pair<unsigned long, unsigned long>>
  scan_points_into_vector() const;
  void traverse_row(unsigned long row, point_reporter_fun_t fun_reporter,
                    void *report_state) const;
  void traverse_column(unsigned long column, point_reporter_fun_t fun_reporter,
                       void *report_state) const;

  struct k2tree_measurement measure_in_memory_size() const;

  ResultTable column_as_table(unsigned long column) const;
  ResultTable row_as_table(unsigned long row) const;

  K2TreeStats k2tree_stats() const;

  bool same_as(const K2TreeMixed &other) const;

  unsigned long write_to_ostream(std::ostream &os) const;
  static K2TreeMixed read_from_istream(std::istream &is);

  size_t size() const;

  bool has_valid_structure() const;

private:
  void clean_up();
};

#endif
