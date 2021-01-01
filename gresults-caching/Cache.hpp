//
// Created by Cristobal Miranda, 2020
//

#ifndef RDFCACHEK2_CACHE_HPP
#define RDFCACHEK2_CACHE_HPP

#include <memory>
#include <string>

#include <request_msg.pb.h>

#include "QueryResult.hpp"
#include <PredicatesCacheManager.hpp>

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
  std::unique_ptr<PredicatesCacheManager> cache_manager;

public:
  using cm_t = std::unique_ptr<PredicatesCacheManager>;
  Cache(std::unique_ptr<PredicatesCacheManager> &&cache_manager);

  CacheStats cache_stats();

  QueryResult run_query(proto_msg::SparqlTree const &query_tree);
  std::string extract_resource(unsigned long index);

  PredicatesCacheManager & get_pcm();

};

#endif // RDFCACHEK2_CACHE_HPP
