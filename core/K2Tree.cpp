//
// Created by Cristobal Miranda, 2020
//

extern "C" {
#include <definitions.h>
#include <memalloc.h>
}

#include <list>
#include <memory>

#include "K2Tree.hpp"
#include "block_serialization.hpp"
#include "block_stats.hpp"
#include "exceptions.hpp"

namespace {

enum BandType {
  BAND_ROW = 0,
  BAND_COLUMN = 1,
};

}

K2Tree::K2Tree(uint32_t tree_depth) : root(create_block()) {
  qs = std::make_unique<struct queries_state>();
  init_queries_state(qs.get(), tree_depth, MAX_NODES_IN_BLOCK, root);
}

K2Tree::K2Tree(uint32_t tree_depth, uint32_t max_node_count)
    : root(create_block()) {
  qs = std::make_unique<struct queries_state>();
  init_queries_state(qs.get(), tree_depth,
                     static_cast<MAX_NODE_COUNT_T>(max_node_count), root);
}

K2Tree::K2Tree(struct block *root, uint32_t tree_depth, uint32_t nodes_in_block)
    : root(root) {
  qs = std::make_unique<struct queries_state>();
  init_queries_state(qs.get(), tree_depth,
                     static_cast<MAX_NODE_COUNT_T>(nodes_in_block), root);
}

K2Tree::K2Tree(K2Tree &&other) noexcept {
  root = nullptr;
  qs = nullptr;
  std::swap(root, other.root);
  std::swap(qs, other.qs);
}

K2Tree &K2Tree::operator=(K2Tree &&rhs) noexcept {
  root = nullptr;
  qs = nullptr;
  std::swap(root, rhs.root);
  std::swap(qs, rhs.qs);
  return *this;
}

K2Tree::~K2Tree() noexcept(false) {
  int err_check;
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
  int already_exists;
  int err_check = insert_point(root, col, row, qs.get(), &already_exists);
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

void K2Tree::write_to_ostream(std::ostream &os) {
  k2tree_data data{};
  data.root = root;
  data.max_node_count = qs->max_nodes_count;
  data.treedepth = qs->treedepth;
  write_tree_to_ostream(data, os);
}

K2Tree K2Tree::read_from_istream(std::istream &is) {
  auto data = read_tree_from_istream(is);
  return K2Tree(data.root, data.treedepth, data.max_node_count);
}

K2TreeStats K2Tree::k2tree_stats() {
  K2TreeStats result{};

  block_rec_occup_ratio_count(root, result);

  auto scanned_points = get_all_points();
  result.number_of_points = static_cast<int>(scanned_points.size());

  return result;
}

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

struct k2tree_measurement K2Tree::measure_in_memory_size() {
  return measure_tree_size(root);
}

struct block *K2Tree::get_root_block() {
  return root;
}
