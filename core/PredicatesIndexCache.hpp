//
// Created by Cristobal Miranda, 2020
//

#ifndef RDFCACHEK2_PREDICATESINDEXCACHE_HPP
#define RDFCACHEK2_PREDICATESINDEXCACHE_HPP

#include <condition_variable>
#include <memory>
#include <mutex>
#include <unordered_map>

#include <parallel/Worker.hpp>

#include "MapOfQueues.hpp"
#include "RDFTriple.hpp"

#include <request_msg.pb.h>

#include "K2Tree.hpp"

struct K2TreeWMutex {
  K2Tree k2tree;
  volatile bool busy;
  explicit K2TreeWMutex(K2Tree &&k2tree)
      : k2tree(std::move(k2tree)), busy(false) {}
};

class PredicatesIndexCache {
  std::unordered_map<uint64_t, std::unique_ptr<K2TreeWMutex>> predicates_map;

public:
  PredicatesIndexCache();
  PredicatesIndexCache(
      std::unordered_map<uint64_t, std::unique_ptr<K2TreeWMutex>> &&k2tree_map);

  void wait_workers();

  void feed_full_k2tree(
      proto_msg::CacheFeedFullK2TreeRequest &cache_feed_full_k2tree_request);

  bool has_predicate(uint64_t predicate_index);
  void add_predicate(uint64_t predicate_index);
  K2Tree &get_k2tree(uint64_t predicate_index);
  K2TreeWMutex &get_k2tree_wmutex(uint64_t predicate_index);

  std::vector<unsigned long> get_predicates_ids();

  void dump_to_file(const std::string &file_path);

  void load_dump_file(const std::string &file_path);
};

class PredicatesIndexCacheBuilder {
  std::unordered_map<uint64_t, std::unique_ptr<K2TreeWMutex>> predicates_map;
  std::mutex m;
  std::condition_variable cv;
  MapOfQueues<RDFTriple> insertion_queue;
  WorkerPool worker_pool;

  unsigned long max_queue_size;
  double measured_insertion_time;

public:
  PredicatesIndexCacheBuilder(int worker_pool_size,
                              unsigned long max_queue_size);

  void insert_point(uint64_t subject_id, uint64_t predicate_id,
                    uint64_t object_id);

  double get_measured_insertion_time();
  void wait_workers();

  bool has_predicate(uint64_t predicate_index);
  void add_predicate(uint64_t predicate_index);

  std::unique_ptr<PredicatesIndexCache> get();
  K2TreeWMutex &get_k2tree_wmutex(uint64_t predicate_index);
};

#endif // RDFCACHEK2_PREDICATESINDEXCACHE_HPP
