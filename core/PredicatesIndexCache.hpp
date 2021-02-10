//
// Created by Cristobal Miranda, 2020
//

#ifndef RDFCACHEK2_PREDICATESINDEXCACHE_HPP
#define RDFCACHEK2_PREDICATESINDEXCACHE_HPP

#include <condition_variable>
#include <memory>
#include <unordered_map>

#include <request_msg.pb.h>

#include "K2TreeMixed.hpp"

#include "PredicatesCacheMetadata.hpp"

class PredicatesIndexCache {
public:
  using predicates_map_t =
      std::unordered_map<uint64_t, std::unique_ptr<K2TreeMixed>>;

private:
  PredicatesCacheMetadata metadata;
  predicates_map_t predicates_map;

  std::unordered_map<uint64_t, size_t> k2tree_sizes;


public:
  PredicatesIndexCache();
  explicit PredicatesIndexCache(std::unordered_map<uint64_t, std::unique_ptr<K2TreeMixed>> &&predicates_map);
  PredicatesIndexCache(
      std::unordered_map<uint64_t, std::unique_ptr<K2TreeMixed>> &&predicates_map,
      PredicatesCacheMetadata &&metadata);
  explicit PredicatesIndexCache(PredicatesCacheMetadata &&metadata);

  void wait_workers();

  bool has_predicate(uint64_t predicate_index);
  void add_predicate(uint64_t predicate_index);
  K2TreeMixed &get_k2tree(uint64_t predicate_index);

  std::vector<unsigned long> get_predicates_ids();

  predicates_map_t &get_predicates_map();

  void dump_to_file(const std::string &file_path);

  void load_dump_file(const std::string &file_path);

  void calculate_sizes();
  size_t get_predicate_k2tree_size(uint64_t predicate_index) const;

  void dump_to_stream(std::ostream &ofs);

  void load_single_predicate(std::istream &ifs, uint64_t predicate_index);
  void discard_in_memory_predicate(uint64_t predicate_index);

  static PredicatesIndexCache from_stream(std::istream &ifs);
  static PredicatesIndexCache
  from_stream_subset(std::istream &ifs,
                     const std::vector<uint64_t> &predicates_to_fetch);


    
};

#endif // RDFCACHEK2_PREDICATESINDEXCACHE_HPP
