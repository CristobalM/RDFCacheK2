//
// Created by Cristobal Miranda, 2020
//

#include "GraphResult.hpp"
#include "utils.hpp"
#include <boost/endian/conversion.hpp>
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

std::string GraphResult::serialize_result() {
  std::vector<uint64_t> predicates;
  for (auto &hmap_item : predicates_indexes) {
    predicates.push_back((uint64_t)hmap_item.first);
  }

  auto predicates_big = predicates;

  std::stringstream ss;

  uint32_t predicates_sz = predicates.size();

  boost::endian::native_to_big_inplace(predicates_sz);
  ss.write(reinterpret_cast<const char *>(&predicates_sz), sizeof(uint32_t));
  boost::endian::big_to_native_inplace(predicates_sz);

  big_to_native_arr(predicates_big.data(), predicates_big.size());
  ss.write(reinterpret_cast<const char *>(predicates_big.data()),
           sizeof(uint64_t) * predicates_big.size());

  for (auto predicate_index : predicates) {
    K2Tree &k2tree = *predicates_indexes[predicate_index];
    k2tree.serialize_tree(ss);
  }
  return ss.str();
}

std::unique_ptr<GraphResult>
GraphResult::from_binary(const std::string &cache_result_binary_string) {
  std::istringstream iss(cache_result_binary_string);
  uint32_t predicates_sz;

  iss.read((char *)&predicates_sz, sizeof(uint32_t));
  boost::endian::big_to_native_inplace(predicates_sz);
  std::vector<uint64_t> predicates(predicates_sz, 0);

  iss.read((char *)predicates.data(), sizeof(uint64_t) * predicates_sz);
  big_to_native_arr(predicates.data(), predicates.size());

  auto cache_result = std::make_unique<GraphResult>();
  for (unsigned long predicate : predicates) {
    cache_result->predicates_indexes[predicate] =
        K2Tree::from_binary_stream(iss);
  }

  return cache_result;
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
