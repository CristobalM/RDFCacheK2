//
// Created by Cristobal Miranda, 2020
//

#ifndef RDFCACHEK2_CACHE_HPP
#define RDFCACHEK2_CACHE_HPP


#include <map>
#include <string>
#include <cache_result/CacheResult.hpp>
#include "FeedData.hpp"
#include "GraphResult.hpp"

class Cache {
  std::map<std::string, GraphResult> results_map;
public:
  Cache();

  bool result_exists(const std::string &label);
  bool feed(FeedData &feed_data);
  GraphResult &get_graph_result(const std::string &label);
};


#endif //RDFCACHEK2_CACHE_HPP
