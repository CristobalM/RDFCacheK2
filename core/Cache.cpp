//
// Created by Cristobal Miranda, 2020
//

#include "Cache.hpp"

Cache::Cache() {}

bool Cache::result_exists(const std::string &label) {
  return results_map.find(label) != results_map.end();
}

GraphResult &Cache::get_graph_result(const std::string &label) {
  return results_map[label];
}

bool Cache::feed(proto_msg::CacheFeedRequest cache_feed_request) {
  auto &query_label = cache_feed_request.query_label();
  results_map[query_label] = GraphResult(cache_feed_request);
  return true;
}

int Cache::results_stored() { return results_map.size(); }

ulong Cache::predicates_stored() {
  ulong result = 0;
  for (auto &hmap_item : results_map) {
    result += hmap_item.second.predicates_count();
  }
  return result;
}

CacheStats Cache::cache_stats() {
  CacheStats result{};
  result.nodes_count = 0;
  result.allocated_u32s = 0;
  result.containers_sz_sum = 0;
  for (auto &hmap_item : results_map) {
    GraphResultStats gstats = hmap_item.second.graph_result_stats();
    result.nodes_count += gstats.nodes_count;
    result.allocated_u32s += gstats.allocated_u32s;
    result.containers_sz_sum += gstats.containers_sz_sum;
  }
  return result;
}
