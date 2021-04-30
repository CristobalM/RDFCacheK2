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
#include "ExprProcessorPersistentData.hpp"
#include "QueryResult.hpp"

class QueryProcessor {
  const PredicatesCacheManager &cm;
  std::unique_ptr<VarIndexManager> vim;

public:
  QueryProcessor(const PredicatesCacheManager &cache_manager);
  QueryResult run_query(proto_msg::SparqlTree const &query_tree);

private:
  std::shared_ptr<ResultTable> process_node(const proto_msg::SparqlNode &node);
  std::shared_ptr<ResultTable>
  process_union_node(const proto_msg::UnionNode &union_node);
  std::shared_ptr<ResultTable>
  process_distinct_node(const proto_msg::DistinctNode &distinct_node);
  std::shared_ptr<ResultTable>
  process_project_node(const proto_msg::ProjectNode &project_node);
  std::shared_ptr<ResultTable>
  process_left_join_node(const proto_msg::LeftJoinNode &left_join_node);
  std::shared_ptr<ResultTable>
  process_join_node(const proto_msg::SparqlNode &join_node);
  std::shared_ptr<ResultTable>
  process_optional_node(const proto_msg::OptionalNode &optional_node);

  void remove_extra_vars_from_table(std::shared_ptr<ResultTable> input_table,
                                    const std::vector<std::string> &vars);
  void remove_repeated_rows(ResultTable &input_table);
  void left_to_right_sort(ResultTable &input_table);
  std::shared_ptr<ResultTable>
  process_filter_node(const proto_msg::FilterNode &node);
};

#endif /* RDFCACHEK2_QUERY_PROCESSOR_HPP */
