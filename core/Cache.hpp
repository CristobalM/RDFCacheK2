//
// Created by Cristobal Miranda, 2020
//

#ifndef RDFCACHEK2_CACHE_HPP
#define RDFCACHEK2_CACHE_HPP

#include "GraphResult.hpp"
#include <cache_result/CacheResult.hpp>
#include <map>
#include <request_msg.pb.h>
#include <string>

struct CacheStats {
  int allocated_u32s;
  int nodes_count;
  int containers_sz_sum;
};

class Cache {
  std::map<std::string, GraphResult> results_map;

public:
  Cache();

  bool result_exists(const std::string &label);
  bool feed(proto_msg::CacheFeedRequest cache_feed_request);
  GraphResult &get_graph_result(const std::string &label);

  int results_stored();

  ulong predicates_stored();

  CacheStats cache_stats();
};

#endif // RDFCACHEK2_CACHE_HPP
