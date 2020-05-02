//
// Created by Cristobal Miranda, 2020
//

#include "Block.hpp"

extern "C"{
#include <vector.h>
#include <definitions.h>
}

Block::Block(uint32_t tree_depth) :
  b(create_block(tree_depth)) {
    init_queries_state(&qs, tree_depth);
}

Block::Block(uint32_t tree_depth, uint32_t max_node_count) : Block(tree_depth) {
  b->max_node_count = max_node_count;
}

Block::~Block() noexcept(false) {

  int err_check = free_rec_block(b);
  if (err_check)
    throw std::runtime_error("free_rec_block: ERROR WHILE FREEING MEMORY, CODE = " + std::to_string(err_check));
  err_check = finish_queries_state(&qs);
  if (err_check)
    throw std::runtime_error("finish_queries_state: ERROR WHILE FREEING MEMORY, CODE = " + std::to_string(err_check));
}

void Block::insert(unsigned long col, unsigned long row) {
  int err_check = insert_point(b, col, row, &qs);
  if (err_check)
    throw std::runtime_error("CANT INSERT, ERROR CODE= " + std::to_string(err_check));
}

bool Block::has(unsigned long col, unsigned long row) {
  int result;
  has_point(b, col, row, &qs, &result);
  return result == 1;
}

std::vector<std::pair<unsigned long, unsigned long>> Block::scan_points() {
  struct vector result;
  int err_check;
  err_check = init_vector_with_capacity(&result, sizeof(unsigned long), 1024);
  if (err_check) throw std::runtime_error("scan_points: CAN'T INITIALIZE VECTOR, CODE: " + std::to_string(err_check));

  err_check = naive_scan_points(b, &qs, &result);
  if (err_check){
    free_vector(&result);
    throw std::runtime_error("scan_points: CODE: " + std::to_string(err_check));
  }

  std::vector<std::pair<unsigned long, unsigned long>> out(result.nof_items);

  for(int i = 0; i < result.nof_items; i++){
    struct pair2dl *current;
    get_element_at(&result, i, (char **)&current);
    out.emplace_back(current->col, current->row);
  }

  free_vector(&result);

  return out;
}
