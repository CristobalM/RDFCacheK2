//
// Created by cristobal on 27-07-21.
//

#ifndef RDFCACHEK2_BLOCK_STATS_HPP
#define RDFCACHEK2_BLOCK_STATS_HPP

extern "C" {
#include <block.h>
#include <block_frontier.h>
#include <block_topology.h>
#include <k2node.h>
#include <queries_state.h>
}

#include "k2tree_stats.hpp"
void block_rec_occup_ratio_count(struct block *b, K2TreeStats &k2tree_stats);
void k2node_rec_occup_ratio_count(struct k2node *node, struct queries_state *qs,
                                  int depth, int cut_depth,
                                  K2TreeMixedStats &k2tree_stats);
#endif // RDFCACHEK2_BLOCK_STATS_HPP
