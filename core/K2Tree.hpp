//
// Created by Cristobal Miranda, 2020
//

#ifndef RDFCACHEK2_K2TREE_HPP
#define RDFCACHEK2_K2TREE_HPP

extern "C" {
#include <block.h>
#include <block_frontier.h>
#include <block_topology.h>
#include <queries_state.h>
}

#include <memory>
#include <sstream>
#include <stdexcept>
#include <string>
#include <vector>

#include <k2tree.pb.h>
#include <mutex>
#include <request_msg.pb.h>

struct K2TreeStats {
  int allocated_u32s;
  int nodes_count;
  int containers_sz_sum;
  int frontier_data;
  int blocks_data;
  int number_of_points;
  int blocks_counted;
};

class K2Tree {
  struct block *root;
  std::unique_ptr<struct queries_state> qs;

public:
  explicit K2Tree(uint32_t tree_depth);
  K2Tree(uint32_t tree_depth, uint32_t max_node_count);
  explicit K2Tree(struct block *root);

  K2Tree(const K2Tree &other) = delete;
  K2Tree &operator=(K2Tree &rhs) = delete;

  K2Tree(K2Tree &&other);
  K2Tree &operator=(K2Tree &&rhs);

  ~K2Tree() noexcept(false);

  void insert(unsigned long col, unsigned long row);
  bool has(unsigned long col, unsigned long row);

  unsigned long get_tree_depth();

  std::vector<std::pair<unsigned long, unsigned long>> get_all_points();

  void scan_points(point_reporter_fun_t fun_reporter, void *report_state);
  void traverse_row(unsigned long row, point_reporter_fun_t fun_reporter,
                    void *report_state);
  void traverse_column(unsigned long column, point_reporter_fun_t fun_reporter,
                       void *report_state);

  K2TreeStats k2tree_stats();

  void produce_proto(proto_msg::K2Tree *to_feed);

  K2Tree(const proto_msg::K2Tree &k2tree_proto);
  K2Tree(proto_msg::CacheFeedFullK2TreeRequest &cache_feed_full_k2tree_request);

  bool same_as(const K2Tree &other);
};

#endif // RDFCACHEK2_K2TREE_HPP
