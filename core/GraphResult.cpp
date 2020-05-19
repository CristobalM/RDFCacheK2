//
// Created by Cristobal Miranda, 2020
//

#include "GraphResult.hpp"
#include <cmath>
#include <request_msg.pb.h>

#include "exceptions.hpp"

GraphResult::GraphResult() = default;

void GraphResult::insert_triple(ulong subject_index, ulong predicate_index,
                                ulong object_index) {
  if (predicates_indexes.find(predicate_index) == predicates_indexes.end()) {
    throw PredicateNotFound(predicate_index);
  }

  predicates_indexes[predicate_index]->insert(subject_index, object_index);
}

bool GraphResult::has_triple(ulong subject_index, ulong predicate_index,
                             ulong object_index) {
  return predicates_indexes.find(predicate_index) != predicates_indexes.end() &&
         predicates_indexes[predicate_index]->has(subject_index, object_index);
}

std::vector<RDFTriple> GraphResult::scan_points() {
  std::vector<RDFTriple> result;
  for (auto &hmap_item : predicates_indexes) {
    ulong predicate_index = hmap_item.first;
    K2Tree &k2tree = *hmap_item.second;
    auto scanned_points = k2tree.scan_points();
    for (auto pair_result : scanned_points) {
      result.push_back(
          {pair_result.first, predicate_index, pair_result.second});
    }
  }
  return result;
}

void GraphResult::insert_predicate(ulong predicate_index,
                                   ulong association_depth) {
  if (predicates_indexes.find(predicate_index) != predicates_indexes.end()) {
    throw PredicateAlreadyExists(predicate_index);
  }
  predicates_indexes[predicate_index] =
      std::make_unique<K2Tree>(association_depth);
}

void GraphResult::insert_predicate(ulong predicate_index) {
  insert_predicate(predicate_index, MAX_ASSOCIATION_DEPTH_DEFAULT);
}

GraphResult::GraphResult(proto_msg::CacheFeedRequest &cache_feed_request) {
  for (const proto_msg::RDFTriple &rdf_triple :
       cache_feed_request.rdf_triples()) {
    if (!has_predicate(rdf_triple.predicate())) {
      insert_predicate(rdf_triple.predicate());
    }
    insert_triple(rdf_triple);
  }
}

bool GraphResult::has_predicate(ulong predicate_index) {
  return predicates_indexes.find(predicate_index) != predicates_indexes.end();
}

void GraphResult::insert_triple(const proto_msg::RDFTriple &rdf_triple) {
  insert_triple(rdf_triple.subject(), rdf_triple.predicate(),
                rdf_triple.object());
}

void GraphResult::produce_proto(proto_msg::GraphResult *graph_result) {
  for (auto &hmap_item : predicates_indexes) {
    auto predicate = (uint64_t)hmap_item.first;
    graph_result->add_predicates(predicate);
    auto &k2tree_ptr = predicates_indexes[predicate];
    proto_msg::K2Tree *k2tree_proto = graph_result->add_k2trees();
    k2tree_ptr->produce_proto(k2tree_proto);
  }
}

GraphResultStats GraphResult::graph_result_stats() {
  GraphResultStats result{};
  result.allocated_u32s = 0;
  result.nodes_count = 0;
  result.containers_sz_sum = 0;
  result.frontier_data = 0;
  result.blocks_data = 0;
  result.number_of_points = 0;
  result.max_points_k2 = 0;
  result.blocks_counted = 0;
  for (auto &hmap_item : predicates_indexes) {
    auto &k2tree_ptr = hmap_item.second;

    K2TreeStats k2tree_stats = k2tree_ptr->k2tree_stats();
    result.allocated_u32s += k2tree_stats.allocated_u32s;
    result.nodes_count += k2tree_stats.nodes_count;
    result.containers_sz_sum += k2tree_stats.containers_sz_sum;
    result.frontier_data += k2tree_stats.frontier_data;
    result.blocks_data += k2tree_stats.blocks_data;
    result.number_of_points += k2tree_stats.number_of_points;
    result.max_points_k2 = std::max(result.max_points_k2, k2tree_stats.number_of_points);
    result.blocks_counted += k2tree_stats.blocks_counted;
  }
  return result;
}

ulong GraphResult::predicates_count() { return predicates_indexes.size(); }
