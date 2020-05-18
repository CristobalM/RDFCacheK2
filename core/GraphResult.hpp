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

  std::string serialize_result();

  static std::unique_ptr<GraphResult>
  from_binary(const std::string &cache_result_binary_string);

  void produce_proto(proto_msg::GraphResult *graph_result);
};

#endif // RDFCACHEK2_GRAPHRESULT_HPP
