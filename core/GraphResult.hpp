//
// Created by Cristobal Miranda, 2020
//

#ifndef RDFCACHEK2_GRAPHRESULT_HPP
#define RDFCACHEK2_GRAPHRESULT_HPP

#include <memory>
#include <unordered_map>
#include <utility>

#include <graph_result.pb.h>
#include <request_msg.pb.h>

#include "K2Tree.hpp"
#include "RDFTriple.hpp"

struct GraphResultStats {
  int allocated_u32s;
  int nodes_count;
  int containers_sz_sum;
  int frontier_data;
  int blocks_data;

  int max_points_k2;
  int number_of_points;
  int blocks_counted;
};

class GraphResult {
  std::unordered_map<ulong, std::unique_ptr<K2Tree>> predicates_indexes;

public:
  GraphResult();
  GraphResult(proto_msg::CacheFeedRequest &cache_feed_request);

  static const ulong MAX_ASSOCIATION_DEPTH_DEFAULT = 32;

  void insert_predicate(ulong predicate_index);
  void insert_predicate(ulong predicate_index, ulong association_depth);
  void insert_triple(ulong subject_index, ulong predicate_index,
                     ulong object_index);
  void insert_triple(const proto_msg::RDFTriple &rdf_triple);
  bool has_triple(ulong subject_index, ulong predicate_index,
                  ulong object_index);
  bool has_predicate(ulong predicate_index);
  std::vector<RDFTriple> scan_points();

  GraphResultStats graph_result_stats();

  void produce_proto(proto_msg::GraphResult *graph_result);

  ulong predicates_count();
};

#endif // RDFCACHEK2_GRAPHRESULT_HPP
