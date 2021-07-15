//
// Created by cristobal on 7/13/21.
//

#ifndef RDFCACHEK2_QUERYPROCESSOR2_HPP
#define RDFCACHEK2_QUERYPROCESSOR2_HPP

#include <memory>
#include <string>

#include <NaiveDynamicStringDictionary.hpp>
#include <PredicatesCacheManager.hpp>
#include <query_processing/expr/BoundVarsMap.hpp>
#include <request_msg.pb.h>

#include "CacheReplacement.hpp"
#include "ExprProcessorPersistentData.hpp"
#include "QueryProcessor.hpp"
#include "QueryResultIterator.hpp"

class QueryProcessor2 {
  const PredicatesCacheManager &cm;
  std::unique_ptr<VarIndexManager> vim;
  std::unique_ptr<NaiveDynamicStringDictionary> extra_str_dict;

public:
  explicit QueryProcessor2(const PredicatesCacheManager &cache_manager);
  QueryProcessor2(
      const PredicatesCacheManager &cache_manager,
      std::unique_ptr<VarIndexManager> &&vim,
      std::unique_ptr<NaiveDynamicStringDictionary> &&extra_str_dict);

  QueryResultIterator run_query(proto_msg::SparqlTree const &query_tree);

private:
  std::shared_ptr<ResultTableIterator>
  process_node(const proto_msg::SparqlNode &node);
  std::shared_ptr<ResultTableIterator>
  process_union_node(const proto_msg::UnionNode &union_node);
  std::shared_ptr<ResultTableIterator>
  process_distinct_node(const proto_msg::DistinctNode &distinct_node);
  std::shared_ptr<ResultTableIterator>
  process_project_node(const proto_msg::ProjectNode &project_node);
  std::shared_ptr<ResultTableIterator>
  process_left_join_node(const proto_msg::LeftJoinNode &left_join_node);
  std::shared_ptr<ResultTableIterator>
  process_join_node(const proto_msg::SparqlNode &join_node);
  std::shared_ptr<ResultTableIterator>
  process_optional_node(const proto_msg::OptionalNode &optional_node);

  static void remove_repeated_rows(ResultTable &input_table);
  void left_to_right_sort(ResultTable &input_table);
  std::shared_ptr<ResultTableIterator>
  process_filter_node(const proto_msg::FilterNode &node);
  std::shared_ptr<ResultTableIterator>
  process_extend_node(const proto_msg::ExtendNode &node);

  std::shared_ptr<ResultTableIterator>
  process_minus_node(const proto_msg::MinusNode &node);
  std::shared_ptr<ResultTableIterator>
  process_sequence_node(const proto_msg::SequenceNode &node);
  std::shared_ptr<ResultTableIterator>
  process_slice_node(const proto_msg::SliceNode &node);
  std::shared_ptr<ResultTableIterator>
  process_order_node(const proto_msg::OrderNode &node);
  std::shared_ptr<std::unordered_map<std::string, unsigned long>>
  create_var_pos_mapping(ResultTableIterator &input_it);
  std::shared_ptr<ResultTableIterator>
  process_path_node(const proto_msg::TripleWithPath &path);
  std::shared_ptr<ResultTableIterator>
  process_table_node(const proto_msg::TableNode &node);
  std::shared_ptr<ResultTableIterator>
  process_group_by_node(const proto_msg::GroupByNode &node);
  std::shared_ptr<ResultTableIterator>
  process_reduced_node(const proto_msg::ReducedNode &node);
  std::set<unsigned long>
  get_var_ids(const std::vector<std::string> &vars_vector);
};

#endif // RDFCACHEK2_QUERYPROCESSOR2_HPP
