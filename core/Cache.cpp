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
  result.frontier_data = 0;
  result.blocks_data = 0;
  result.max_points_k2 = 0;
  result.number_of_points_avg = 0;
  result.blocks_counted = 0;

  for (auto &hmap_item : results_map) {
    GraphResultStats gstats = hmap_item.second.graph_result_stats();
    result.nodes_count += gstats.nodes_count;
    result.allocated_u32s += gstats.allocated_u32s;
    result.containers_sz_sum += gstats.containers_sz_sum;
    result.frontier_data += gstats.frontier_data;
    result.blocks_data += gstats.blocks_data;
    result.max_points_k2 = std::max(gstats.max_points_k2, result.max_points_k2);
    result.number_of_points_avg += gstats.number_of_points;
    result.blocks_counted += gstats.blocks_counted;
  }
  if(results_map.size() > 0){
    result.number_of_points_avg /= results_map.size();
  }

  return result;
}
