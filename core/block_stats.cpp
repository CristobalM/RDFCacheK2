//
// Created by cristobal on 27-07-21.
//

#include "block_stats.hpp"

void block_rec_occup_ratio_count(struct block *b, K2TreeStats &k2tree_stats) {
  k2tree_stats.allocated_u32s += b->container_size;
  k2tree_stats.nodes_count += b->nodes_count;
  k2tree_stats.containers_sz_sum += sizeof(struct block);

  k2tree_stats.frontier_data += b->children * sizeof(NODES_BV_T);
  k2tree_stats.blocks_data += b->children * sizeof(struct block *);
  k2tree_stats.blocks_counted += 1;

  struct block **children = b->children_blocks;
  for (int i = 0; i < b->children; i++) {
    struct block *child_block = children[i];
    block_rec_occup_ratio_count(child_block, k2tree_stats);
  }
}
