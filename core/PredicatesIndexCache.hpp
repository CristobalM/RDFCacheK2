//
// Created by Cristobal Miranda, 2020
//

#ifndef RDFCACHEK2_PREDICATESINDEXCACHE_HPP
#define RDFCACHEK2_PREDICATESINDEXCACHE_HPP

#include <memory>
#include <unordered_map>

#include <request_msg.pb.h>

#include "K2Tree.hpp"

class PredicatesIndexCache {
  std::unordered_map<uint64_t, std::unique_ptr<K2Tree>> predicates_map;

public:
  PredicatesIndexCache();

  void feed_full_k2tree(
      proto_msg::CacheFeedFullK2TreeRequest &cache_feed_full_k2tree_request);

  bool has_predicate(uint64_t predicate_index);
  K2Tree &get_k2tree(uint64_t predicate_index);

  void dump_to_file(const std::string &file_path);

  void load_dump_file(const std::string &file_path);
};

#endif // RDFCACHEK2_PREDICATESINDEXCACHE_HPP
