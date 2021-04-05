//
// Created by Cristobal Miranda, 2020
//

#ifndef RDFCACHEK2_QUERY_PROCESSOR_HPP
#define RDFCACHEK2_QUERY_PROCESSOR_HPP

#include <memory>
#include <string>

#include <PredicatesCacheManager.hpp>
#include <request_msg.pb.h>

#include "CacheReplacement.hpp"
#include "QueryResult.hpp"

class QueryProcessor {
  const PredicatesCacheManager &cache_manager;

public:
  QueryProcessor(const PredicatesCacheManager &cache_manager);
  QueryResult run_query(proto_msg::SparqlTree const &query_tree);
};

#endif /* RDFCACHEK2_QUERY_PROCESSOR_HPP */
