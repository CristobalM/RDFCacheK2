//
// Created by Cristobal Miranda, 2020
//

#ifndef RDFCACHEK2_QUERY_PROCESSOR_HPP
#define RDFCACHEK2_QUERY_PROCESSOR_HPP

#include <memory>
#include <string>

#include <NaiveDynamicStringDictionary.hpp>
#include <PredicatesCacheManager.hpp>
#include <query_processing/expr/BoundVarsMap.hpp>
#include <request_msg.pb.h>

#include "CacheReplacement.hpp"
#include "ExprProcessorPersistentData.hpp"
#include "QueryResult.hpp"

class QueryProcessor {
  const PredicatesCacheManager &cm;
  std::unique_ptr<VarIndexManager> vim;
  std::unique_ptr<NaiveDynamicStringDictionary> extra_str_dict;

public:
  QueryProcessor(const PredicatesCacheManager &cache_manager);
  QueryProcessor(
      const PredicatesCacheManager &cache_manager,
      std::unique_ptr<VarIndexManager> &&vim,
      std::unique_ptr<NaiveDynamicStringDictionary> &&extra_str_dict);

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
  std::shared_ptr<ResultTable>
  process_extend_node(const proto_msg::ExtendNode &node);

  std::shared_ptr<ResultTable>
  process_minus_node(const proto_msg::MinusNode &node);
  std::shared_ptr<ResultTable>
  process_sequence_node(const proto_msg::SequenceNode &node);
  std::shared_ptr<ResultTable>
  process_slice_node(const proto_msg::SliceNode &node);
  std::shared_ptr<ResultTable>
  process_order_node(const proto_msg::OrderNode &node);
  std::unordered_map<std::string, unsigned long>
  create_var_pos_mapping(ResultTable &table);
  std::shared_ptr<ResultTable>
  process_path_node(const proto_msg::TripleWithPath &path);
  std::shared_ptr<ResultTable>
  process_table_node(const proto_msg::TableNode &node);
  std::shared_ptr<ResultTable>
  process_group_by_node(const proto_msg::GroupByNode &node);
  std::shared_ptr<ResultTable>
  process_reduced_node(const proto_msg::ReducedNode &node);
};

#endif /* RDFCACHEK2_QUERY_PROCESSOR_HPP */
