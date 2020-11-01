#include "K2TreeMixed.hpp"

#include "block_serialization.hpp"
#include "serialization_util.hpp"
#include <algorithm>
#include <vector>

K2TreeMixed::K2TreeMixed(uint32_t treedepth)
    : K2TreeMixed(treedepth, MAX_NODES_IN_BLOCK) {}

K2TreeMixed::K2TreeMixed(uint32_t treedepth, uint32_t max_node_count)
    : K2TreeMixed(treedepth, max_node_count, std::max(treedepth / 4, 1U)) {}
K2TreeMixed::K2TreeMixed(uint32_t treedepth, uint32_t max_node_count,
                         uint32_t cut_depth)
    : K2TreeMixed(create_k2node(), treedepth, max_node_count, cut_depth) {}

K2TreeMixed::K2TreeMixed(struct k2node *root, uint32_t treedepth,
                         uint32_t max_node_count, uint32_t cut_depth)
    : root(root) {
  st = std::make_unique<struct k2qstate>();
  init_k2qstate(st.get(), treedepth, max_node_count, cut_depth);
}

void K2TreeMixed::clean_up() {
  if (root) {
    free_rec_k2node(root, 0, st->cut_depth);
  }
  root = nullptr;
  if (st) {
    clean_k2qstate(st.get());
  }
  st = nullptr;
}

K2TreeMixed::K2TreeMixed(K2TreeMixed &&other) {
  clean_up();
  std::swap(root, other.root);
  std::swap(st, other.st);
}

K2TreeMixed &K2TreeMixed::operator=(K2TreeMixed &&other) {
  clean_up();
  std::swap(root, other.root);
  std::swap(st, other.st);
  return *this;
}

K2TreeMixed::~K2TreeMixed() { clean_up(); }

void K2TreeMixed::insert(unsigned long col, unsigned long row) {
  k2node_insert_point(root, col, row, st.get());
}

bool K2TreeMixed::has(unsigned long col, unsigned long row) {
  int result;
  k2node_has_point(root, col, row, st.get(), &result);
  return (bool)result;
}

unsigned long K2TreeMixed::get_tree_depth() { return st->k2tree_depth; }

std::vector<std::pair<unsigned long, unsigned long>>
K2TreeMixed::get_all_points() {
  struct vector_pair2dl_t result;
  vector_pair2dl_t__init_vector(&result);
  k2node_naive_scan_points(root, st.get(), &result);
  std::vector<std::pair<unsigned long, unsigned long>> out;

  for (long i = 0; i < result.nof_items; i++) {
    struct pair2dl current = result.data[i];
    out.emplace_back(current.col, current.row);
  }
  vector_pair2dl_t__free_vector(&result);
  return out;
}

std::vector<unsigned long> K2TreeMixed::get_row(unsigned long row) {
  struct vector_pair2dl_t result;
  vector_pair2dl_t__init_vector(&result);
  k2node_report_row(root, row, st.get(), &result);
  std::vector<unsigned long> out;
  for (long i = 0; i < result.nof_items; i++) {
    unsigned long col = result.data[i].col;
    out.push_back(col);
  }
  vector_pair2dl_t__free_vector(&result);
  return out;
}

std::vector<unsigned long> K2TreeMixed::get_column(unsigned long col) {
  struct vector_pair2dl_t result;
  vector_pair2dl_t__init_vector(&result);
  k2node_report_column(root, col, st.get(), &result);
  std::vector<unsigned long> out;
  for (long i = 0; i < result.nof_items; i++) {
    unsigned long row = result.data[i].row;
    out.push_back(row);
  }
  vector_pair2dl_t__free_vector(&result);
  return out;
}

void K2TreeMixed::scan_points(point_reporter_fun_t fun_reporter,
                              void *report_state) {
  k2node_scan_points_interactively(root, st.get(), fun_reporter, report_state);
}
void K2TreeMixed::traverse_row(unsigned long row,
                               point_reporter_fun_t fun_reporter,
                               void *report_state) {
  k2node_report_row_interactively(root, row, st.get(), fun_reporter,
                                  report_state);
}
void K2TreeMixed::traverse_column(unsigned long column,
                                  point_reporter_fun_t fun_reporter,
                                  void *report_state) {
  k2node_report_column_interactively(root, column, st.get(), fun_reporter,
                                     report_state);
}

struct k2tree_measurement K2TreeMixed::measure_in_memory_size() {
  return k2node_measure_tree_size(root, st->cut_depth);
}

ResultTable K2TreeMixed::column_as_table(unsigned long) {
  throw "column_as_table not implemented";
}

ResultTable K2TreeMixed::row_as_table(unsigned long) {
  throw "row_as_table not implemented";
}

K2TreeStats K2TreeMixed::k2tree_stats() {
  throw "k2tree_stats not implemented";
}

bool same_k2node(struct k2node *lhs, struct k2node *rhs, uint32_t current_depth,
                 uint32_t cut_depth) {
  if (current_depth < cut_depth) {
    for (int i = 0; i < 4; i++) {
      if ((bool)lhs->k2subtree.children[i] !=
          (bool)lhs->k2subtree.children[i]) {
        return false;
      }
    }
    for (int i = 0; i < 4; i++) {
      if (!same_k2node(lhs->k2subtree.children[i], rhs->k2subtree.children[i],
                       current_depth + 1, cut_depth)) {
        return false;
      }
    }
    return true;
  }

  return same_blocks(lhs->k2subtree.block_child, rhs->k2subtree.block_child);
}

bool K2TreeMixed::same_as(const K2TreeMixed &other) {
  return same_k2node(root, other.root, 0, st->cut_depth);
}

int count_k2nodes_wchildren(struct k2node *node, uint32_t current_depth,
                            uint32_t cut_depth) {
  if (current_depth < cut_depth) {
    int sum = 0;
    for (int i = 0; i < 4; i++) {
      if (node->k2subtree.children[i]) {
        sum += count_k2nodes_wchildren(node->k2subtree.children[i],
                                       current_depth + 1, cut_depth);
      }
    }
    return sum + 1;
  }
  return 0;
}

void serialize_to_vec_with_k2node_ptrs(struct k2node *node,
                                       uint32_t current_depth,
                                       uint32_t cut_depth,
                                       std::vector<uint32_t> &data,
                                       uint32_t &current_node_location) {
  if (current_depth < cut_depth) {
    int container_pos =
        4 * current_node_location / 32; // 4*current_node_location/32;
    int subpos_bits_start = (4 * current_node_location) % 32;
    uint32_t &container = data[container_pos];
    current_node_location++;
    for (int i = 0; i < 4; i++) {
      if (node->k2subtree.children[i]) {
        uint32_t bit_location = (31 - (subpos_bits_start + i));
        container |= (1U << bit_location);
      }
    }
    for (int i = 0; i < 4; i++) {
      if (node->k2subtree.children[i]) {
        serialize_to_vec_with_k2node_ptrs(node->k2subtree.children[i],
                                          current_depth + 1, cut_depth, data,
                                          current_node_location);
      }
    }
  }
}

void write_blocks_from_k2nodes(struct k2node *node, uint32_t current_depth,
                               uint32_t cut_depth, std::ostream &os,
                               struct k2qstate *st) {
  if (current_depth < cut_depth) {
    for (int i = 0; i < 4; i++) {
      if (node->k2subtree.children[i]) {
        write_blocks_from_k2nodes(node->k2subtree.children[i],
                                  current_depth + 1, cut_depth, os, st);
      }
    }
  } else {
    k2tree_data serialization_data;
    serialization_data.root = node->k2subtree.block_child;
    serialization_data.max_node_count = st->qs.max_nodes_count;
    serialization_data.treedepth = st->qs.treedepth;
    write_tree_to_ostream(serialization_data, os);
  }
}

void K2TreeMixed::write_to_ostream(std::ostream &os) {
  int k2nodes_count_wchildren = count_k2nodes_wchildren(root, 0, st->cut_depth);
  int bits_count = k2nodes_count_wchildren * 4;
  const int bits_per_container = (sizeof(uint32_t) * 8);
  int containers_count = (bits_count / bits_per_container) +
                         ((bits_count % bits_per_container == 0) ? 0 : 1);
  std::vector<uint32_t> containers(containers_count, 0);
  uint32_t current_node_location = 0;

  serialize_to_vec_with_k2node_ptrs(root, 0, st->cut_depth, containers,
                                    current_node_location);
  write_u32(os, st->k2tree_depth);
  write_u32(os, st->cut_depth);
  write_u32(os, st->qs.max_nodes_count);
  write_u32(os, containers_count);
  for (int i = 0; i < containers_count; i++) {
    write_u32(os, containers[i]);
  }
  write_blocks_from_k2nodes(root, 0, st->cut_depth, os, st.get());
}

struct k2node *deserialize_k2node_tree(std::istream &is,
                                       std::vector<uint32_t> &containers,
                                       uint32_t current_depth,
                                       uint32_t cut_depth,
                                       uint32_t &current_node_location) {
  struct k2node *node = create_k2node();
  if (current_depth < cut_depth) {
    int container_pos = 4 * current_node_location / 32;
    int subpos_bits_start = (4 * current_node_location) % 32;
    uint32_t container = containers[container_pos];
    current_node_location++;
    for (int i = 0; i < 4; i++) {
      if (container & (1U << (31 - (subpos_bits_start + i)))) {
        node->k2subtree.children[i] =
            deserialize_k2node_tree(is, containers, current_depth + 1,
                                    cut_depth, current_node_location);
      }
    }
  } else {
    k2tree_data subtree_deserialized = read_tree_from_istream(is);
    node->k2subtree.block_child = subtree_deserialized.root;
  }

  return node;
}

K2TreeMixed K2TreeMixed::read_from_istream(std::istream &is) {
  uint32_t k2tree_depth = read_u32(is);
  uint32_t cut_depth = read_u32(is);
  uint32_t max_nodes_count = read_u32(is);
  uint32_t containers_count = read_u32(is);
  std::vector<uint32_t> containers(containers_count, 0);
  for (uint32_t i = 0; i < containers_count; i++) {
    containers[i] = read_u32(is);
  }
  uint32_t current_node_location = 0;
  struct k2node *root = deserialize_k2node_tree(is, containers, 0, cut_depth,
                                                current_node_location);
  return K2TreeMixed(root, k2tree_depth, max_nodes_count, cut_depth);
}
