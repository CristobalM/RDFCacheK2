#ifndef RDFCACHEK2_K2TREE_MIXED_HPP
#define RDFCACHEK2_K2TREE_MIXED_HPP

#include <istream>
#include <memory>
#include <utility>
#include <vector>

#include "K2QStateWrapper.hpp"
#include "K2TreeScanner.hpp"
#include "k2tree_stats.hpp"

#include "MemorySegment.hpp"
#include "serialization_util.hpp"

extern "C" {
#include "definitions.h"
#include "k2node.h"
}

struct k2node;
class K2TreeScanner;

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
  // std::unique_ptr<struct k2qstate> st;
  uint64_t points_count;
  uint32_t tree_depth;
  uint32_t cut_depth;
  uint32_t max_nodes_count;

public:
  explicit K2TreeMixed(uint32_t treedepth);
  K2TreeMixed(uint32_t treedepth, uint32_t max_node_count);
  K2TreeMixed(uint32_t treedepth, uint32_t max_node_count, uint32_t cut_depth);
  K2TreeMixed(struct k2node *root, uint32_t treedepth, uint32_t max_node_count,
              uint32_t cut_depth, uint64_t points_count);

  K2TreeMixed(K2TreeConfig config);

  K2TreeMixed(const K2TreeMixed &other);
  K2TreeMixed &operator=(const K2TreeMixed &other);

  K2TreeMixed(K2TreeMixed &&other) noexcept;
  K2TreeMixed &operator=(K2TreeMixed &&other) noexcept;

  ~K2TreeMixed();

  struct k2node *get_root_k2node();

  void insert(unsigned long col, unsigned long row, K2QStateWrapper &stw);
  void remove(unsigned long col, unsigned long row, K2QStateWrapper &stw);
  bool has(unsigned long col, unsigned long row, K2QStateWrapper &stw) const;

  void remove(unsigned long col, unsigned long row);
  void insert(unsigned long col, unsigned long row);
  bool has(unsigned long col, unsigned long row) const;

  unsigned long get_tree_depth() const;

  std::vector<std::pair<unsigned long, unsigned long>>
  get_all_points(K2QStateWrapper &stw);

  std::vector<std::pair<unsigned long, unsigned long>> get_all_points();

  void scan_points(point_reporter_fun_t fun_reporter, void *report_state,
                   K2QStateWrapper &stw) const;
  void scan_points(point_reporter_fun_t fun_reporter, void *report_state) const;
  void traverse_row(unsigned long row, point_reporter_fun_t fun_reporter,
                    void *report_state, K2QStateWrapper &stw) const;
  void traverse_row(unsigned long row, point_reporter_fun_t fun_reporter,
                    void *report_state) const;
  void traverse_column(unsigned long column, point_reporter_fun_t fun_reporter,
                       void *report_state, K2QStateWrapper &stw) const;
  void traverse_column(unsigned long column, point_reporter_fun_t fun_reporter,
                       void *report_state) const;

  struct k2tree_measurement measure_in_memory_size() const;

  k2tree_measurement k2tree_stats() const;

  bool identical_structure_as(const K2TreeMixed &other) const;

  unsigned long write_to_ostream(std::ostream &os) const;
  static K2TreeMixed read_from_istream(std::istream &is);

  size_t size() const;

  bool has_valid_structure(K2QStateWrapper &stw) const;
  bool has_valid_structure() const;

  enum BandType { COLUMN_BAND_TYPE = 0, ROW_BAND_TYPE = 1 };

  std::unique_ptr<K2TreeScanner> create_full_scanner();
  std::unique_ptr<K2TreeScanner>
  create_band_scanner(unsigned long band, K2TreeScanner::BandType band_type);

  std::unique_ptr<K2TreeScanner> create_empty_scanner();

  K2QStateWrapper create_k2qw() const;

  static K2TreeMixed read_from_istream(std::istream &istream,
                                       MemorySegment *memory_segment);

  // This was made for debugging/testing purposes, to check that two copies are
  // not sharing any references
  bool shares_any_reference_to(K2TreeMixed &other);

private:
  void clean_up();
};

#endif
