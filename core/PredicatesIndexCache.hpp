//
// Created by Cristobal Miranda, 2020
//

#ifndef RDFCACHEK2_PREDICATESINDEXCACHE_HPP
#define RDFCACHEK2_PREDICATESINDEXCACHE_HPP

#include <condition_variable>
#include <memory>
#include <mutex>
#include <unordered_map>

#include "MapOfQueues.hpp"
#include "RDFTriple.hpp"

#include <request_msg.pb.h>

#include "K2Tree.hpp"

class PredicatesIndexCache {
public:
  using predicates_map_t =
      std::unordered_map<uint64_t, std::unique_ptr<K2Tree>>;

private:
  predicates_map_t predicates_map;

public:
  PredicatesIndexCache();
  PredicatesIndexCache(
      std::unordered_map<uint64_t, std::unique_ptr<K2Tree>> &&k2tree_map);

  void wait_workers();

  void feed_full_k2tree(
      proto_msg::CacheFeedFullK2TreeRequest &cache_feed_full_k2tree_request);

  bool has_predicate(uint64_t predicate_index);
  void add_predicate(uint64_t predicate_index);
  K2Tree &get_k2tree(uint64_t predicate_index);

  std::vector<unsigned long> get_predicates_ids();

  predicates_map_t &get_predicates_map();

  void dump_to_file(const std::string &file_path);

  void load_dump_file(const std::string &file_path);
};

class PredicatesIndexCacheBuilder {
  PredicatesIndexCache::predicates_map_t predicates_map;
  MapOfQueues<RDFTriple> insertion_queue;

  int worker_pool_size;
  unsigned long max_queue_size;
  double measured_insertion_time;

public:
  PredicatesIndexCacheBuilder(int worker_pool_size,
                              unsigned long max_queue_size);

  void insert_point(uint64_t subject_id, uint64_t predicate_id,
                    uint64_t object_id);

  double get_measured_insertion_time();

  void finish();

  bool has_predicate(uint64_t predicate_index);
  void add_predicate(uint64_t predicate_index);

  std::unique_ptr<PredicatesIndexCache> get();
  K2Tree &get_k2tree(uint64_t predicate_index);

private:
  void insert_batch();
};

#endif // RDFCACHEK2_PREDICATESINDEXCACHE_HPP
