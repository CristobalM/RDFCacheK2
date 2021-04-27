//
// Created by Cristobal Miranda, 2020
//

#include <chrono>
#include <sstream>

#include "Cache.hpp"
#include "CacheReplacementFactory.hpp"

Cache::Cache(std::shared_ptr<PredicatesCacheManager> &cache_manager,
             CacheReplacement::STRATEGY cache_replacement_strategy,
             size_t memory_budget_bytes)
    : cache_manager(cache_manager),
      cache_replacement(CacheReplacementFactory::select_strategy(
          cache_replacement_strategy, memory_budget_bytes, cache_manager)) {}

QueryResult Cache::run_query(proto_msg::SparqlTree const &query_tree) {
  return QueryProcessor(*cache_manager)
      .run_query(query_tree);
}

RDFResource Cache::extract_resource(unsigned long index) const {
  return cache_manager->extract_resource(index);
}

PredicatesCacheManager &Cache::get_pcm() { return *cache_manager; }
