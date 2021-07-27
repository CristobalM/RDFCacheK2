//
// Created by cristobal on 27-07-21.
//

#include "block_stats.hpp"

void block_rec_occup_ratio_count(struct block *b, K2TreeStats &k2tree_stats) {
  k2tree_stats.allocated_u32s += b->container_size;
  k2tree_stats.nodes_count += b->nodes_count;
  k2tree_stats.containers_sz_sum += sizeof(struct block);

  k2tree_stats.frontier_data += b->children * sizeof(uint32_t);
  k2tree_stats.blocks_data += b->children * sizeof(struct block *);
  k2tree_stats.blocks_counted += 1;

  struct block **children = b->children_blocks;
  for (int i = 0; i < b->children; i++) {
    struct block *child_block = children[i];
    block_rec_occup_ratio_count(child_block, k2tree_stats);
  }
}

void k2node_rec_occup_ratio_count(struct k2node *node, struct queries_state *qs,
                                  int depth, int cut_depth,
                                  K2TreeMixedStats &k2tree_stats) {
  if (depth == cut_depth) {
    struct block *block = node->k2subtree.block_child;
    if (block) {
      block_rec_occup_ratio_count(block, k2tree_stats.inner_blocks_stats);
      k2tree_stats.total_blocks++;

      scan_points_interactively(
          block, qs,
          [](unsigned long, unsigned long, void *report_state) {
            auto &inner_stats = *reinterpret_cast<K2TreeStats *>(report_state);
            inner_stats.number_of_points++;
          },
          &k2tree_stats.inner_blocks_stats);
    }
    return;
  }

  for (int i = 0; i < 4; i++) {
    struct k2node *child = node->k2subtree.children[i];
    if (child != nullptr) {
      k2node_rec_occup_ratio_count(child, qs, depth + 1, cut_depth,
                                   k2tree_stats);
      k2tree_stats.total_ptrs++;
    }
  }
}
