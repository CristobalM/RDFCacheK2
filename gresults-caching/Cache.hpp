//
// Created by Cristobal Miranda, 2020
//

#ifndef RDFCACHEK2_CACHE_HPP
#define RDFCACHEK2_CACHE_HPP

#include <memory>
#include <string>

#include <PredicatesCacheManager.hpp>
#include <query_processing/ExprProcessorPersistentData.hpp>
#include <request_msg.pb.h>

#include "CacheReplacement.hpp"

#include "ICacheSettings.hpp"

#include "query_processing/QueryProcessor.hpp"
#include "query_processing/QueryResult.hpp"

struct CacheStats {
  int allocated_u32s;
  int nodes_count;
  int containers_sz_sum;
  int frontier_data;
  int blocks_data;

  int max_points_k2;
  int number_of_points_avg;
  int blocks_counted;
};

class Cache {
  std::shared_ptr<PredicatesCacheManager> cache_manager;

  CacheReplacement cache_replacement;

  ExprProcessorPersistentData expr_processor_persistent_data;

public:
  using cm_t = std::shared_ptr<PredicatesCacheManager>;
  using pcm_t = PredicatesCacheManager;
  Cache(std::shared_ptr<PredicatesCacheManager> &cache_manager,
        CacheReplacement::STRATEGY cache_replacement_strategy,
        size_t memory_budget_bytes);

  CacheStats cache_stats();

  QueryResult run_query(proto_msg::SparqlTree const &query_tree);
  RDFResource extract_resource(unsigned long index) const;

  PredicatesCacheManager &get_pcm();
};

#endif // RDFCACHEK2_CACHE_HPP
