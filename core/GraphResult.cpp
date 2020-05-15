//
// Created by Cristobal Miranda, 2020
//

#include <cmath>
#include "GraphResult.hpp"

#include "exceptions.hpp"


GraphResult::GraphResult() = default;

void GraphResult::insert_triple(ulong subject_index, ulong predicate_index, ulong object_index) {
  if (predicates_indexes.find(predicate_index) == predicates_indexes.end()) {
    throw PredicateNotFound(predicate_index);
  }

  predicates_indexes[predicate_index]->insert(subject_index, object_index);
}

bool GraphResult::has_triple(ulong subject_index, ulong predicate_index, ulong object_index) {
  return predicates_indexes.find(predicate_index) != predicates_indexes.end() &&
         predicates_indexes[predicate_index]->has(subject_index, object_index);
}

std::vector<RDFTriple> GraphResult::scan_points() {
  std::vector<RDFTriple> result;
  for (auto &hmap_item : predicates_indexes) {
    ulong predicate_index = hmap_item.first;
    K2Tree &k2tree = *hmap_item.second;
    auto scanned_points = k2tree.scan_points();
    for (auto pair_result: scanned_points) {
      result.push_back({pair_result.first, predicate_index, pair_result.second});
    }
  }
  return result;
}

void GraphResult::insert_predicate(ulong predicate_index, ulong max_associated_entities) {
  if (predicates_indexes.find(predicate_index) != predicates_indexes.end()) {
    throw PredicateAlreadyExists(predicate_index);
  }
  predicates_indexes[predicate_index] = std::make_unique<K2Tree>(std::ceil(std::log2(max_associated_entities)));
}

std::string GraphResult::serialize_result() {
  std::vector<uint64_t> predicates;
  for (auto &hmap_item : predicates_indexes){
    predicates.push_back((uint64_t)hmap_item.first);
  }

  std::stringstream ss;

  uint32_t predicates_sz = predicates.size();

  ss.write(reinterpret_cast<const char *>(&predicates_sz), sizeof(uint32_t));
  ss.write(reinterpret_cast<const char *>(predicates.data()), sizeof(uint64_t) * predicates.size());


  for(auto predicate_index : predicates){
    K2Tree &k2tree = *predicates_indexes[predicate_index];
    k2tree.serialize_tree(ss);
  }
  return ss.str();
}

std::unique_ptr<GraphResult> GraphResult::from_binary(const std::string &cache_result_binary_string) {
  std::istringstream iss(cache_result_binary_string);
  uint32_t predicates_sz;

  iss.read((char *)&predicates_sz, sizeof(uint32_t));
  std::vector<uint64_t> predicates(predicates_sz, 0);

  iss.read((char *)predicates.data(), sizeof(uint64_t) * predicates_sz);

  auto cache_result = std::make_unique<GraphResult>();
  for(unsigned long predicate : predicates){
    cache_result->predicates_indexes[predicate] = K2Tree::from_binary_stream(iss);
  }

  return cache_result;
}
