//
// Created by Cristobal Miranda, 2020
//

extern "C" {
#include <definitions.h>
#include <memalloc.h>
}

#include <cmath>
#include <iterator>
#include <list>
#include <memory>
#include <unordered_map>

#include "K2Tree.hpp"
#include "exceptions.hpp"
#include "serialization_util.hpp"

namespace {

enum BandType {
  BAND_ROW = 0,
  BAND_COLUMN = 1,
};

}

bool same_blocks(const struct block *lhs, const struct block *rhs);

bool same_block_frontiers(const struct block *lhs, const struct block *rhs);

bool same_block_topologies(const struct block *lhs, const struct block *rhs);

bool same_bvs(const struct block *lhs, const struct block *rhs);

bool same_vectors(const struct vector_uint32_t &lhs,
                  const struct vector_uint32_t &rhs);

void fill_vector_with_coordinates(std::vector<unsigned long> &target,
                                  struct vector_pair2dl_t *input,
                                  BandType band_type);

std::vector<unsigned long> get_k2tree_band(unsigned long band_index,
                                           BandType band_type,
                                           struct block *root,
                                           struct queries_state *qs);

void adjust_blocks(std::list<struct block *> &blocks);

K2Tree::K2Tree(uint32_t tree_depth) : root(create_block()) {
  qs = std::make_unique<struct queries_state>();
  init_queries_state(qs.get(), tree_depth, MAX_NODES_IN_BLOCK, root);
}

K2Tree::K2Tree(uint32_t tree_depth, uint32_t max_node_count)
    : root(create_block()) {
  qs = std::make_unique<struct queries_state>();
  init_queries_state(qs.get(), tree_depth, max_node_count, root);
}

K2Tree::K2Tree(struct block *root, uint32_t tree_depth, uint32_t nodes_in_block)
    : root(root) {
  qs = std::make_unique<struct queries_state>();
  init_queries_state(qs.get(), tree_depth, nodes_in_block, root);
}

K2Tree::K2Tree(K2Tree &&other) {
  root = nullptr;
  qs = nullptr;
  std::swap(root, other.root);
  std::swap(qs, other.qs);
}

K2Tree &K2Tree::operator=(K2Tree &&rhs) {
  root = nullptr;
  qs = nullptr;
  std::swap(root, rhs.root);
  std::swap(qs, rhs.qs);
  return *this;
}

K2Tree::~K2Tree() noexcept(false) {
  int err_check = 0;
  if (root) {
    err_check = free_rec_block(root);
    if (err_check)
      throw std::runtime_error(
          "free_rec_block: ERROR WHILE FREEING MEMORY, CODE = " +
          std::to_string(err_check));
  }
  if (qs) {
    err_check = finish_queries_state(qs.get());
    if (err_check)
      throw std::runtime_error(
          "finish_queries_state: ERROR WHILE FREEING MEMORY, CODE = " +
          std::to_string(err_check));
  }
}

void K2Tree::insert(unsigned long col, unsigned long row) {
  int err_check = insert_point(root, col, row, qs.get());
  if (err_check)
    throw std::runtime_error("insert: CANT INSERT POINT " +
                             std::to_string(col) + ", " + std::to_string(row) +
                             ", ERROR CODE= " + std::to_string(err_check));
}

bool K2Tree::has(unsigned long col, unsigned long row) {
  int result;
  int err_check = has_point(root, col, row, qs.get(), &result);
  if (err_check)
    throw std::runtime_error("has: ERROR WHILE SEARCHING POINT " +
                             std::to_string(col) + ", " + std::to_string(row) +
                             ", ERROR CODE= " + std::to_string(err_check));
  return result == 1;
}

std::vector<std::pair<unsigned long, unsigned long>> K2Tree::get_all_points() {
  struct vector_pair2dl_t result {};
  int err_check;
  err_check = vector_pair2dl_t__init_vector_with_capacity(&result, 1024);
  if (err_check)
    throw std::runtime_error("scan_points: CAN'T INITIALIZE VECTOR, CODE: " +
                             std::to_string(err_check));

  err_check = naive_scan_points(root, qs.get(), &result);
  if (err_check) {
    vector_pair2dl_t__free_vector(&result);
    throw std::runtime_error("scan_points: CODE: " + std::to_string(err_check));
  }

  std::vector<std::pair<unsigned long, unsigned long>> out;

  for (int i = 0; i < result.nof_items; i++) {
    struct pair2dl current = result.data[i];
    out.emplace_back(current.col, current.row);
  }

  vector_pair2dl_t__free_vector(&result);

  return out;
}

void K2Tree::scan_points(point_reporter_fun_t fun_reporter,
                         void *report_state) {
  int err_check =
      scan_points_interactively(root, qs.get(), fun_reporter, report_state);
  if (err_check) {
    throw std::runtime_error("scan_points (interactive): CODE: " +
                             std::to_string(err_check));
  }
}

void K2Tree::traverse_row(unsigned long row, point_reporter_fun_t fun_reporter,
                          void *report_state) {
  int err_check =
      report_row_interactively(root, row, qs.get(), fun_reporter, report_state);
  if (err_check) {
    throw std::runtime_error("traverse_row (interactive): CODE: " +
                             std::to_string(err_check));
  }
}

void K2Tree::traverse_column(unsigned long column,
                             point_reporter_fun_t fun_reporter,
                             void *report_state) {
  int err_check = report_column_interactively(root, column, qs.get(),
                                              fun_reporter, report_state);
  if (err_check) {
    throw std::runtime_error("traverse_column (interactive): CODE: " +
                             std::to_string(err_check));
  }
}

std::vector<unsigned long> K2Tree::get_row(unsigned long row) {
  return get_k2tree_band(row, BAND_ROW, root, qs.get());
}

std::vector<unsigned long> K2Tree::get_column(unsigned long column) {
  return get_k2tree_band(column, BAND_COLUMN, root, qs.get());
}

void fill_vector_with_coordinates(std::vector<unsigned long> &target,
                                  struct vector_pair2dl_t *input,
                                  BandType band_type) {
  for (int i = 0; i < input->nof_items; i++) {
    struct pair2dl current = input->data[i];
    switch (band_type) {
    case BAND_ROW:
      target.emplace_back(current.row);
      break;
    case BAND_COLUMN:
      target.emplace_back(current.col);
      break;
    }
  }
}

void write_block_to_ostream(struct block *b, std::ostream &os) {
  uint16_t nodes_count = b->nodes_count;
  uint16_t children = b->children;
  uint16_t container_size = b->container_size;

  write_u16(os, nodes_count);
  write_u16(os, children);
  write_u16(os, container_size);

  NODES_BV_T *frontier_preorders = b->preorders;
  for (uint32_t i = 0; i < children; i++) {
    uint16_t current_preorder = frontier_preorders[i];
    write_u16(os, current_preorder);
  }

  uint32_t *data = b->container;

  for (int i = 0; i < (int)container_size; i++) {
    uint32_t current_sub_block = data[i];
    write_u32(os, current_sub_block);
  }
}

struct block *read_block_from_istream(std::istream &is) {
  uint16_t nodes_count = read_u16(is);
  uint16_t children = read_u16(is);
  uint16_t container_size = read_u16(is);

  struct block *new_block = create_block();

  init_block_topology(new_block, nodes_count);
  init_block_frontier_with_capacity(new_block, children);

  new_block->children = children;

  for (uint32_t j = 0; j < children; j++) {
    uint16_t frontier_element = read_u16(is);
    new_block->preorders[j] = frontier_element;
  }

  new_block->container_size = container_size;

  uint32_t *container = k2tree_alloc_u32array((int)new_block->container_size);
  new_block->container = reinterpret_cast<BVCTYPE *>(container);
  for (uint32_t sub_block_i = 0; sub_block_i < container_size; sub_block_i++) {
    new_block->container[sub_block_i] = read_u32(is);
  }
  new_block->nodes_count = nodes_count;

  return new_block;
}

uint32_t traverse_tree_write_to_ostream(struct block *b, std::ostream &os) {
  struct block **child_blocks = b->children_blocks;

  uint32_t blocks_counted = 1;

  for (int i = 0; i < b->children; i++) {
    struct block *current_child_block = child_blocks[i];
    blocks_counted += traverse_tree_write_to_ostream(current_child_block, os);
  }

  write_block_to_ostream(b, os);
  return blocks_counted;
}

void K2Tree::write_to_ostream(std::ostream &os) {
  write_u16(os, qs->max_nodes_count);
  write_u16(os, qs->treedepth);
  auto start_pos = os.tellp();
  write_u32(os, 0);
  uint32_t blocks_count = traverse_tree_write_to_ostream(root, os);
  auto curr_pos = os.tellp();
  os.seekp(start_pos);
  write_u32(os, blocks_count);
  os.seekp(curr_pos);
}

K2Tree K2Tree::read_from_istream(std::istream &is) {
  uint16_t max_node_count = read_u16(is);
  uint16_t tree_depth = read_u16(is);
  uint32_t blocks_count = read_u32(is);

  if (blocks_count == 0) {
    throw std::runtime_error(
        "K2Tree::read_from_istream: input stream has zero blocks");
  }

  std::list<struct block *> blocks_to_adjust;
  struct block *current_block;

  for (uint32_t i = 0; i < blocks_count; i++) {
    current_block = read_block_from_istream(is);
    blocks_to_adjust.push_back(current_block);
    adjust_blocks(blocks_to_adjust);
  }
  // root is always the last block in the serialization
  return K2Tree(current_block, tree_depth, max_node_count);
}

void adjust_blocks(std::list<struct block *> &blocks) {

  auto pointer = blocks.rbegin();
  struct block *current_block = *pointer;
  pointer++;
  int frontier_sz = current_block->children;
  for (int i = frontier_sz - 1; i >= 0; i--) {
    struct block *current_child = *pointer;
    current_block->children_blocks[i] = current_child;
    blocks.erase(--(pointer.base()));
  }
}

void rec_occup_ratio_count(struct block *b, K2TreeStats &k2tree_stats) {
  k2tree_stats.allocated_u32s += b->container_size;
  k2tree_stats.nodes_count += b->nodes_count;
  k2tree_stats.containers_sz_sum += sizeof(struct block);

  k2tree_stats.frontier_data += b->children * sizeof(uint32_t);
  k2tree_stats.blocks_data += b->children * sizeof(struct block *);
  k2tree_stats.blocks_counted += 1;

  struct block **children = b->children_blocks;
  for (int i = 0; i < b->children; i++) {
    struct block *child_block = children[i];
    rec_occup_ratio_count(child_block, k2tree_stats);
  }
}

K2TreeStats K2Tree::k2tree_stats() {
  K2TreeStats result{};
  result.allocated_u32s = 0;
  result.nodes_count = 0;
  result.containers_sz_sum = 0;
  result.frontier_data = 0;
  result.blocks_data = 0;
  result.number_of_points = 0;
  result.blocks_counted = 0;

  rec_occup_ratio_count(root, result);

  auto scanned_points = get_all_points();
  result.number_of_points = scanned_points.size();

  return result;
}

unsigned long K2Tree::get_tree_depth() { return qs->treedepth; }

bool K2Tree::same_as(const K2Tree &other) {
  return same_blocks(root, other.root);
}

ResultTable K2Tree::column_as_table(unsigned long column) {
  std::list<std::vector<unsigned long>> data;

  traverse_column(
      column,
      [](unsigned long, unsigned long row, void *_data) {
        auto &data =
            *reinterpret_cast<std::list<std::vector<unsigned long>> *>(_data);
        data.push_back({row});
      },
      &data);

  return ResultTable(column, std::move(data));
}

ResultTable K2Tree::row_as_table(unsigned long row) {
  std::list<std::vector<unsigned long>> data;

  traverse_row(
      row,
      [](unsigned long col, unsigned long, void *_data) {
        auto &data =
            *reinterpret_cast<std::list<std::vector<unsigned long>> *>(_data);
        data.push_back({col});
      },
      &data);

  return ResultTable(row, std::move(data));
}

bool same_blocks(const struct block *lhs, const struct block *rhs) {
  auto bf_result = same_block_frontiers(lhs, rhs);
  auto bt_result = same_block_topologies(lhs, rhs);
  auto result = bf_result && bt_result;
  return result;
}

bool same_block_topologies(const struct block *lhs, const struct block *rhs) {
  return lhs->nodes_count == rhs->nodes_count && same_bvs(lhs, rhs);
}

bool same_bvs(const struct block *lhs, const struct block *rhs) {
  if (!(lhs->container_size == rhs->container_size &&
        lhs->nodes_count == rhs->nodes_count)) {
    return false;
  }

  for (uint32_t i = 0; i < lhs->container_size; i++) {
    if (lhs->container[i] != rhs->container[i]) {
      return false;
    }
  }
  return true;
}

bool same_block_frontiers(const struct block *lhs, const struct block *rhs) {

  if (lhs->children != rhs->children) {
    return false;
  }

  for (int i = 0; i < lhs->children; i++) {
    if (lhs->preorders[i] != rhs->preorders[i])
      return false;
  }

  for (int i = 0; i < lhs->children; i++) {
    if (!same_blocks(lhs->children_blocks[i], rhs->children_blocks[i])) {
      return false;
    }
  }

  return true;
}

std::vector<unsigned long> get_k2tree_band(unsigned long band_index,
                                           BandType band_type,
                                           struct block *root,
                                           struct queries_state *qs) {
  struct vector_pair2dl_t result {};
  int err_check;
  err_check = vector_pair2dl_t__init_vector_with_capacity(&result, 1024);
  if (err_check)
    throw std::runtime_error(
        "get_k2tree_band: CAN'T INITIALIZE VECTOR, CODE: " +
        std::to_string(err_check));

  switch (band_type) {
  case BAND_ROW:
    err_check = report_row(root, band_index, qs, &result);
    break;
  case BAND_COLUMN:
    err_check = report_column(root, band_index, qs, &result);
    break;
  default:
    throw std::runtime_error("get_k2tree_band: UNKNOWN band_type " +
                             std::to_string(band_type));
  }

  if (err_check) {
    vector_pair2dl_t__free_vector(&result);
    throw std::runtime_error("get_k2tree_band: CODE: " +
                             std::to_string(err_check));
  }

  std::vector<unsigned long> out;
  switch (band_type) {
  case BAND_ROW:
    fill_vector_with_coordinates(out, &result, BAND_COLUMN);
    break;
  case BAND_COLUMN:
    fill_vector_with_coordinates(out, &result, BAND_ROW);
    break;
  }

  vector_pair2dl_t__free_vector(&result);

  return out;
}

struct k2tree_measurement K2Tree::measure_in_memory_size() {
  return measure_tree_size(root);
}
