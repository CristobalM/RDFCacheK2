//
// Created by Cristobal Miranda, 2020
//

#include <unordered_map>
#include <cmath>

#include "K2Tree.hpp"
#include "exceptions.hpp"

extern "C" {
#include <vector.h>
#include <definitions.h>
#include <memalloc.h>
}




K2Tree::K2Tree(uint32_t tree_depth) :
        root(create_block(tree_depth)) {
  init_queries_state(&qs, tree_depth);
}

K2Tree::K2Tree(uint32_t tree_depth, uint32_t max_node_count) : K2Tree(tree_depth) {
  root->max_node_count = max_node_count;
}

K2Tree::K2Tree(struct block *root) : root(root) {
  init_queries_state(&qs, root->tree_depth);
}


K2Tree::~K2Tree() noexcept(false) {

  int err_check = free_rec_block(root);
  if (err_check)
    throw std::runtime_error("free_rec_block: ERROR WHILE FREEING MEMORY, CODE = " + std::to_string(err_check));
  err_check = finish_queries_state(&qs);
  if (err_check)
    throw std::runtime_error("finish_queries_state: ERROR WHILE FREEING MEMORY, CODE = " + std::to_string(err_check));
}

void K2Tree::insert(unsigned long col, unsigned long row) {
  int err_check = insert_point(root, col, row, &qs);
  if (err_check)
    throw std::runtime_error(
            "insert: CANT INSERT POINT " + std::to_string(col) + ", " + std::to_string(row) + ", ERROR CODE= " +
            std::to_string(err_check));
}

bool K2Tree::has(unsigned long col, unsigned long row) {
  int result;
  int err_check = has_point(root, col, row, &qs, &result);
  if (err_check)
    throw std::runtime_error(
            "has: ERROR WHILE SEARCHING POINT " + std::to_string(col) + ", " + std::to_string(row) + ", ERROR CODE= " +
            std::to_string(err_check));
  return result == 1;
}

std::vector<std::pair<unsigned long, unsigned long>> K2Tree::scan_points() {
  struct vector result{};
  int err_check;
  err_check = init_vector_with_capacity(&result, sizeof(unsigned long), 1024);
  if (err_check) throw std::runtime_error("scan_points: CAN'T INITIALIZE VECTOR, CODE: " + std::to_string(err_check));

  err_check = naive_scan_points(root, &qs, &result);
  if (err_check) {
    free_vector(&result);
    throw std::runtime_error("scan_points: CODE: " + std::to_string(err_check));
  }

  std::vector<std::pair<unsigned long, unsigned long>> out;

  for (int i = 0; i < result.nof_items; i++) {
    struct pair2dl *current;
    get_element_at(&result, i, (char **) &current);
    out.emplace_back(current->col, current->row);
  }

  free_vector(&result);

  return out;
}

void serialize_block(struct block *b, std::vector<uint64_t> &children_ids, std::stringstream &ss){

  uint32_t block_index = b->block_index;
  uint32_t block_depth = b->block_depth;
  uint32_t topology_sz = b->bt->bv->size_in_bits;
  uint32_t frontier_sz = b->bf->frontier.nof_items;
  uint32_t container_sz = b->bt->bv->container_size;
  uint32_t max_nodes_count = b->max_node_count;

  BVCTYPE *container = b->bt->bv->container;

  struct vector *frontier_preorders = &b->bf->frontier;

  ss.write(reinterpret_cast<const char *>(&block_index), sizeof(uint32_t));
  ss.write(reinterpret_cast<const char *>(&block_depth), sizeof(uint32_t));
  ss.write(reinterpret_cast<const char *>(&topology_sz), sizeof(uint32_t));
  ss.write(reinterpret_cast<const char *>(&frontier_sz), sizeof(uint32_t));
  ss.write(reinterpret_cast<const char *>(&container_sz), sizeof(uint32_t));
  ss.write(reinterpret_cast<const char *>(&max_nodes_count), sizeof(uint32_t));

  ss.write(reinterpret_cast<const char *>(container), sizeof(BVCTYPE) * container_sz);
  ss.write(reinterpret_cast<const char *>(frontier_preorders->data), sizeof(uint32_t) * frontier_sz);
  ss.write(reinterpret_cast<const char *>(children_ids.data()), sizeof(uint64_t) * frontier_sz);
}

/**
 * Recursive function to serialize the blocks in the k2tree
 * @param b root block
 * @param ss stringstream where to put data
 * @return number of blocks scanned
 */
uint32_t serialize_block_rec(struct block *b, std::stringstream &ss){
  struct vector child_blocks = b->bf->blocks;

  std::vector<uint64_t > children_ids;

  uint32_t blocks_counted = 1;

  for(int i = 0; i < child_blocks.nof_items; i++){
    struct block *current_child_block;
    get_element_at(&child_blocks, i, (char **)&current_child_block);
    children_ids.push_back((uint64_t )current_child_block->block_index);
    blocks_counted += serialize_block_rec(current_child_block, ss);
  }

  serialize_block(b, children_ids, ss);
  return blocks_counted;
}

void K2Tree::serialize_tree(std::stringstream &stream) {
  std::stringstream ss;

  uint32_t blocks_qty = serialize_block_rec(root, ss);
  uint32_t tree_depth = root->tree_depth;
  stream.write(reinterpret_cast<const char *>(&tree_depth), sizeof(uint32_t));
  stream.write(reinterpret_cast<const char *>(&blocks_qty), sizeof(uint32_t));

  /* TODO: check if this affect performance too much */
  stream << ss.rdbuf();

}

std::unique_ptr<K2Tree> K2Tree::from_binary_stream(std::istringstream &k2tree_binary_stream) {

  std::istringstream &iss = k2tree_binary_stream;
  uint32_t tree_depth;
  uint32_t blocks_qty;

  std::unordered_map<uint64_t, struct block *> id_hmap;

  iss.read((char *)&tree_depth, sizeof(uint32_t));
  iss.read((char *)&blocks_qty, sizeof(uint32_t));

  std::vector<struct block *> initialized_blocks;

  struct block * root_block = nullptr;

  for(uint32_t i = 0; i < blocks_qty; i++ ){
    uint32_t block_index;
    uint32_t block_depth;
    uint32_t topology_sz;
    uint32_t frontier_sz;
    uint32_t container_sz;
    uint32_t max_node_count;

    struct block *new_block = k2tree_alloc_block();
    new_block->bt = k2tree_alloc_block_topology();
    new_block->bf = k2tree_alloc_block_frontier();

    iss.read((char *)&block_index, sizeof(uint32_t));
    iss.read((char *)&block_depth, sizeof(uint32_t));
    iss.read((char *)&topology_sz, sizeof(uint32_t));
    iss.read((char *)&frontier_sz, sizeof(uint32_t));
    iss.read((char *)&container_sz, sizeof(uint32_t));
    iss.read((char *)&max_node_count, sizeof(uint32_t));


    struct u32array_alloc alloc_container = k2tree_alloc_u32array((int)container_sz);
    iss.read((char *)alloc_container.data, sizeof(BVCTYPE) * container_sz);

    auto frontier_preorders_capacity = (int)(1u << (unsigned int)std::ceil(std::log2(frontier_sz)));
    init_vector_with_capacity(&new_block->bf->frontier, sizeof(uint32_t), frontier_preorders_capacity);
    iss.read(new_block->bf->frontier.data, sizeof(uint32_t) * frontier_sz);

    std::vector<uint64_t> children_ids(frontier_sz, 0);
    iss.read((char *)children_ids.data(), sizeof(uint64_t) * frontier_sz);

    /* initializes current block */

    new_block->block_depth = block_depth;
    new_block->tree_depth = tree_depth;
    new_block->max_node_count = max_node_count;

    if(block_depth == 0){
      root_block = new_block;
    }

    new_block->block_index = block_index;

    /* initializes current topology */
    new_block->bt->nodes_count = topology_sz / 4;

    new_block->bt->bv = k2tree_alloc_bitvector();
    new_block->bt->bv->container = alloc_container.data;
    new_block->bt->bv->size_in_bits = topology_sz;
    new_block->bt->bv->container_size = container_sz;
    new_block->bt->bv->alloc_tag = alloc_container.size;
    /* initializes current frontier */
    new_block->bf->frontier.nof_items = frontier_sz;
    init_vector_with_capacity(&new_block->bf->blocks, sizeof(struct block *), frontier_preorders_capacity);
    new_block->bf->blocks.nof_items = frontier_sz;
    for(int child_position = 0; i < children_ids.size(); i++){
      auto child_id = children_ids[child_position];
      struct block *current_block = id_hmap[child_id];
      set_element_at(&new_block->bf->blocks, (char *)&current_block, child_position);
    }

    id_hmap[block_index] = new_block;
    initialized_blocks.push_back(new_block);
  }

  if(root_block == nullptr){
    throw RootNotFound();
  }

  for(auto *b : initialized_blocks){
    b->root = root_block;
  }

  return std::make_unique<K2Tree>(root_block);
}

