#ifndef K2TREE_STATS_HPP
#define K2TREE_STATS_HPP

struct K2TreeStats {
  int allocated_u32s;
  int nodes_count;
  int containers_sz_sum;
  int frontier_data;
  int blocks_data;
  int number_of_points;
  int blocks_counted;
};

#endif
