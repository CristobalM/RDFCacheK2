#ifndef _PREDICATES_INDEX_CACHE_BUILDER_HPP_
#define _PREDICATES_INDEX_CACHE_BUILDER_HPP_

#include <memory>

#include "PredicatesIndexCache.hpp"
#include "MapOfQueues.hpp"
#include "RDFTriple.hpp"
#include "K2TreeMixed.hpp"

class PredicatesIndexCacheBuilder {
  PredicatesIndexCache::predicates_map_t predicates_map;
  MapOfQueues<RDFTriple> insertion_queue;

  int worker_pool_size;
  unsigned long max_queue_size;
  double measured_insertion_time;

  K2TreeConfig config;

public:
  PredicatesIndexCacheBuilder(int worker_pool_size,
                              unsigned long max_queue_size, K2TreeConfig config);

  void insert_point(uint64_t subject_id, uint64_t predicate_id,
                    uint64_t object_id);

  double get_measured_insertion_time();

  void finish();

  bool has_predicate(uint64_t predicate_index);
  void add_predicate(uint64_t predicate_index);

  std::unique_ptr<PredicatesIndexCache> get();
  K2TreeMixed &get_k2tree(uint64_t predicate_index);

private:
  void insert_batch();
};

#endif /* _PREDICATES_INDEX_CACHE_BUILDER_HPP_ */
