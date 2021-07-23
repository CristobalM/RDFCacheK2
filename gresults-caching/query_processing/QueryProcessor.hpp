//
// Created by cristobal on 7/13/21.
//

#ifndef RDFCACHEK2_QUERYPROCESSOR_HPP
#define RDFCACHEK2_QUERYPROCESSOR_HPP

#include <memory>
#include <string>

#include <NaiveDynamicStringDictionary.hpp>
#include <PredicatesCacheManager.hpp>
#include <TimeControl.hpp>
#include <query_processing/expr/BoundVarsMap.hpp>
#include <request_msg.pb.h>

#include "CacheReplacement.hpp"
#include "ParsingUtils.hpp"
#include "QProc.hpp"
#include "QueryResultIterator.hpp"
#include "VarBindingQProc.hpp"

class QueryProcessor : public QProc {
  std::shared_ptr<PredicatesCacheManager> cm;
  std::shared_ptr<VarIndexManager> vim;
  std::shared_ptr<NaiveDynamicStringDictionary> extra_str_dict;
  TimeControl &time_control;

public:
  QueryProcessor(std::shared_ptr<PredicatesCacheManager> cache_manager,
                 TimeControl &time_control);
  QueryProcessor(std::shared_ptr<PredicatesCacheManager> cache_manager,
                 std::shared_ptr<VarIndexManager> vim,
                 std::shared_ptr<NaiveDynamicStringDictionary> extra_str_dict,
                 TimeControl &time_control);

  QueryResultIterator run_query(proto_msg::SparqlTree const &query_tree);
  VarIndexManager &get_vim() override;
  std::shared_ptr<PredicatesCacheManager> get_cache_manager() override;

  QueryResultIterator
  run_query(const proto_msg::SparqlNode &proto_node,
            std::shared_ptr<VarBindingQProc> var_binding_qproc);
  std::shared_ptr<VarIndexManager> get_vim_ptr() override;
  std::shared_ptr<NaiveDynamicStringDictionary>
  get_extra_str_dict_ptr() override;

private:
  std::shared_ptr<ResultTableIterator>
  process_node(const proto_msg::SparqlNode &node,
               std::shared_ptr<VarBindingQProc> var_binding_qproc) override;
  std::shared_ptr<ResultTableIterator>
  process_union_node(const proto_msg::UnionNode &union_node,
                     const std::shared_ptr<VarBindingQProc> &var_binding_qproc);
  std::shared_ptr<ResultTableIterator>
  process_distinct_node(const proto_msg::DistinctNode &distinct_node,
                        std::shared_ptr<VarBindingQProc> var_binding_qproc);
  std::shared_ptr<ResultTableIterator>
  process_project_node(const proto_msg::ProjectNode &project_node,
                       std::shared_ptr<VarBindingQProc> var_binding_qproc);
  std::shared_ptr<ResultTableIterator> process_left_join_node(
      const proto_msg::LeftJoinNode &left_join_node,
      const std::shared_ptr<VarBindingQProc> &var_binding_qproc);
  std::shared_ptr<ResultTableIterator> process_optional_node(
      const proto_msg::OptionalNode &optional_node,
      const std::shared_ptr<VarBindingQProc> &var_binding_qproc);

  void remove_repeated_rows(ResultTable &input_table);
  void left_to_right_sort(ResultTable &input_table);
  std::shared_ptr<ResultTableIterator> process_filter_node(
      const proto_msg::FilterNode &node,
      const std::shared_ptr<VarBindingQProc> &var_binding_qproc);
  std::shared_ptr<ResultTableIterator> process_extend_node(
      const proto_msg::ExtendNode &node,
      const std::shared_ptr<VarBindingQProc> &var_binding_qproc);

  std::shared_ptr<ResultTableIterator>
  process_minus_node(const proto_msg::MinusNode &node,
                     const std::shared_ptr<VarBindingQProc> &var_binding_qproc);
  std::shared_ptr<ResultTableIterator>
  process_sequence_node(const proto_msg::SequenceNode &node,
                        std::shared_ptr<VarBindingQProc> var_binding_qproc);
  std::shared_ptr<ResultTableIterator>
  process_slice_node(const proto_msg::SliceNode &node,
                     std::shared_ptr<VarBindingQProc> var_binding_qproc);
  std::shared_ptr<ResultTableIterator>
  process_order_node(const proto_msg::OrderNode &node,
                     const std::shared_ptr<VarBindingQProc> &var_binding_qproc);
  std::shared_ptr<std::unordered_map<std::string, unsigned long>>
  create_var_pos_mapping(ResultTableIterator &input_it);
  std::shared_ptr<ResultTableIterator>
  process_path_node(const proto_msg::TripleWithPath &path);
  std::shared_ptr<ResultTableIterator>
  process_table_node(const proto_msg::TableNode &node,
                     std::shared_ptr<VarBindingQProc> var_binding_qproc);
  std::shared_ptr<ResultTableIterator>
  process_group_by_node(const proto_msg::GroupByNode &node);
  std::shared_ptr<ResultTableIterator>
  process_reduced_node(const proto_msg::ReducedNode &node);
  std::set<unsigned long>
  get_var_ids(const std::vector<std::string> &vars_vector);
  void left_to_right_sort_vec(ResultTableVector &result_table_vector);
  std::shared_ptr<ResultTable>
  convert_to_result_table_list(ResultTableVector &result_table_vector);
  RDFResource resource_from_proto_term(const proto_msg::RDFTerm &term);
  RDFResourceType resource_type_from_term(const proto_msg::RDFTerm &term);
  unsigned long id_from_resource(RDFResource &resource);
  std::string sanitize_term(const proto_msg::RDFTerm &term);
  std::string sanitize_number_term(const proto_msg::RDFTerm &term);
  std::string sanitize_boolean_term(const proto_msg::RDFTerm &term);
  std::string sanitize_date_term(const proto_msg::RDFTerm &term);
  std::string infer_number_type_str(std::string &input_string_number);
  std::string to_lowercase(std::string input);
};

#endif // RDFCACHEK2_QUERYPROCESSOR_HPP
