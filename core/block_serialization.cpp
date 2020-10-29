extern "C"{
    #include <block.h>
    #include <memalloc.h>
}
#include <stdexcept>

#include "block_serialization.hpp"
#include "serialization_util.hpp"

k2tree_data read_tree_from_istream(std::istream &is){ 
  uint16_t max_node_count = read_u16(is);
  uint16_t treedepth = read_u16(is);
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
  k2tree_data out;
  out.max_node_count = max_node_count;
  out.root = current_block;
  out.treedepth = treedepth;
  return out;
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

void write_tree_to_ostream(k2tree_data data, std::ostream &os){
  write_u16(os, data.max_node_count);
  write_u16(os, data.treedepth);
  auto start_pos = os.tellp();
  write_u32(os, 0);
  uint32_t blocks_count = traverse_tree_write_to_ostream(data.root, os);
  auto curr_pos = os.tellp();
  os.seekp(start_pos);
  write_u32(os, blocks_count);
  os.seekp(curr_pos);
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
