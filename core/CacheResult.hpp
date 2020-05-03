//
// Created by Cristobal Miranda, 2020
//

#ifndef RDFCACHEK2_CACHERESULT_HPP
#define RDFCACHEK2_CACHERESULT_HPP


#include <unordered_map>
#include <memory>
#include <utility>

#include "K2Tree.hpp"
#include "RDFTriple.hpp"

class CacheResult {
  std::unordered_map<ulong, std::unique_ptr<K2Tree>> predicates_indexes;

public:
  CacheResult();

  void insert_predicate(ulong predicate_index, ulong max_associated_entities);
  void insert_triple(ulong subject_index, ulong predicate_index, ulong object_index);
  bool has_triple(ulong subject_index, ulong predicate_index, ulong object_index);
  std::vector<RDFTriple> scan_points();

  std::string serialize_result();

  static std::unique_ptr<CacheResult> from_binary(const std::string &cache_result_binary_string);

};


#endif //RDFCACHEK2_CACHERESULT_HPP
