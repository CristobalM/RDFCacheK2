

//
// Created by Cristobal Miranda, 2020
//

#include <chrono>
#include <query_processing/expr/ExprEval.hpp>
#include <stdexcept>
#include <string>
#include <unordered_map>
#include <utility>

//#include "BGPProcessor.hpp"
#include "BGPProcessor2.hpp"
#include "InnerJoinProcessor.hpp"
#include "InnerJoinProcessor2.hpp"
#include "OptionalProcessor2.hpp"
#include "OrderNodeProcessor.hpp"
#include "QueryProcessor2.hpp"
#include "QueryResultIterator.hpp"
#include "ResultTableFilterIterator.hpp"
#include "ResultTableIteratorExtend.hpp"
#include "ResultTableIteratorFromMaterialized.hpp"
#include "ResultTableIteratorMinus.hpp"
#include "ResultTableIteratorProject.hpp"
#include "ResultTableIteratorSlice.hpp"
#include "ResultTableIteratorUnion.hpp"
#include "Term.hpp"
#include "VarIndexManager.hpp"
#include "VarLazyBinding.hpp"

QueryProcessor2::QueryProcessor2(const PredicatesCacheManager &cache_manager)
    : QueryProcessor2(cache_manager, std::make_unique<VarIndexManager>(),
                      nullptr) {}

std::shared_ptr<ResultTableIterator>
QueryProcessor2::process_join_node(const proto_msg::SparqlNode &join_node) {

  std::shared_ptr<ResultTableIterator> result_table;

  switch (join_node.node_case()) {
  case proto_msg::SparqlNode::NodeCase::kBgpNode:
    result_table = BGPProcessor2(join_node.bgp_node(), cm, *vim).execute_it();
    break;
  case proto_msg::SparqlNode::NodeCase::kLeftJoinNode:
    result_table = process_left_join_node(join_node.left_join_node());
    break;
  default:
    throw std::runtime_error("Unsupported nodetype on process_join_node: " +
                             std::to_string(join_node.node_case()));
  }

  return result_table;
}

std::shared_ptr<ResultTableIterator> QueryProcessor2::process_left_join_node(
    const proto_msg::LeftJoinNode &left_join_node) {
  auto left_table = process_node(left_join_node.left_node());
  auto right_table = process_node(left_join_node.right_node());
  auto resulting_table =
      OptionalProcessor2(left_table, right_table).execute_it();

  std::vector<const proto_msg::ExprNode *> nodes;

  for (int i = 0; i < left_join_node.expr_list_size(); i++) {
    const auto &expr_node = left_join_node.expr_list(i);
    nodes.push_back(&expr_node);
  }

  if (extra_str_dict)
    extra_str_dict = std::make_unique<NaiveDynamicStringDictionary>();

  return std::make_shared<ResultTableFilterIterator>(
      resulting_table, *vim, nodes, cm, *extra_str_dict);
}

std::shared_ptr<ResultTableIterator> QueryProcessor2::process_project_node(
    const proto_msg::ProjectNode &project_node) {
  std::vector<std::string> vars;
  for (int i = 0; i < project_node.vars_size(); i++) {
    vars.push_back(project_node.vars(i));
  }

  for (const auto &var : vars) {
    vim->assign_index_if_not_found(var);
  }

  auto result_table = process_node(project_node.sub_op());

  auto var_ids = get_var_ids(vars);
  return std::make_shared<ResultTableIteratorProject>(std::move(result_table),
                                                      var_ids);
}

std::shared_ptr<ResultTableIterator>
QueryProcessor2::process_node(const proto_msg::SparqlNode &node) {

  switch (node.node_case()) {
  case proto_msg::SparqlNode::NodeCase::kProjectNode:
    return process_project_node(node.project_node());
  case proto_msg::SparqlNode::NodeCase::kBgpNode:
    return BGPProcessor2(node.bgp_node(), cm, *vim).execute_it();
  case proto_msg::SparqlNode::NodeCase::kLeftJoinNode:
    return process_left_join_node(node.left_join_node());
  case proto_msg::SparqlNode::NodeCase::kUnionNode:
    return process_union_node(node.union_node());
  case proto_msg::SparqlNode::NodeCase::kDistinctNode:
    return process_distinct_node(node.distinct_node());
  case proto_msg::SparqlNode::NodeCase::kOptionalNode:
    return process_optional_node(node.optional_node());
  case proto_msg::SparqlNode::NodeCase::kFilterNode:
    return process_filter_node(node.filter_node());
  case proto_msg::SparqlNode::NodeCase::kExtendNode:
    return process_extend_node(node.extend_node());
  case proto_msg::SparqlNode::kMinusNode:
    return process_minus_node(node.minus_node());
  case proto_msg::SparqlNode::kSequenceNode:
    return process_sequence_node(node.sequence_node());
  case proto_msg::SparqlNode::kSliceNode:
    return process_slice_node(node.slice_node());
  case proto_msg::SparqlNode::kOrderNode:
    return process_order_node(node.order_node());
  default:
    throw std::runtime_error("Unsupported nodetype on process_node: " +
                             std::to_string(node.node_case()));
  }
}

QueryResultIterator
QueryProcessor2::run_query(proto_msg::SparqlTree const &query_tree) {
  auto result = process_node(query_tree.root());
  if (extra_str_dict)
    return QueryResultIterator(result, std::move(vim),
                               std::move(extra_str_dict));
  return QueryResultIterator(result, std::move(vim));
}

std::shared_ptr<ResultTableIterator>
QueryProcessor2::process_union_node(const proto_msg::UnionNode &union_node) {
  //  UnionProcessor union_processor;
  //  for (int i = 0; i < union_node.nodes_list_size(); i++) {
  //    auto &node = union_node.nodes_list(i);
  //    auto table = process_node(node);
  //    union_processor.combine_table(std::move(table));
  //  }
  //  return union_processor.get_result();

  std::vector<std::shared_ptr<ResultTableIterator>> its;
  for (int i = 0; i < union_node.nodes_list_size(); i++) {
    auto &node = union_node.nodes_list(i);
    auto it = process_node(node);
    its.push_back(std::move(it));
  }

  return std::make_shared<ResultTableIteratorUnion>(std::move(its));
}

std::shared_ptr<ResultTableIterator> QueryProcessor2::process_distinct_node(
    const proto_msg::DistinctNode &distinct_node) {
  auto table = process_node(distinct_node.sub_node())->materialize();
  left_to_right_sort(*table);
  remove_repeated_rows(*table);
  return std::make_shared<ResultTableIteratorFromMaterialized>(
      std::move(table));
}

std::shared_ptr<ResultTableIterator> QueryProcessor2::process_optional_node(
    const proto_msg::OptionalNode &optional_node) {
  auto left_table = process_node(optional_node.left_node());
  auto right_table = process_node(optional_node.right_node());
  return OptionalProcessor2(left_table, right_table).execute_it();
}

void QueryProcessor2::remove_repeated_rows(ResultTable &input_table) {
  auto left_it = input_table.data.begin();

  for (auto it = std::next(left_it); it != input_table.data.end();) {
    if (*it == *left_it) {
      auto next_it = std::next(it);
      input_table.data.erase(it);
      it = next_it;
    } else {
      left_it = it;
      it++;
    }
  }
}

void QueryProcessor2::left_to_right_sort(ResultTable &input_table) {

  input_table.data.sort([](const std::vector<unsigned long> &lhs,
                           const std::vector<unsigned long> &rhs) {
    for (size_t i = 0; i < lhs.size(); i++) {
      auto lhs_i = lhs.at(i);
      auto rhs_i = rhs.at(i);
      if (lhs_i == rhs_i)
        continue;
      return lhs_i < rhs_i;
    }
    return true;
  });
}

std::shared_ptr<ResultTableIterator>
QueryProcessor2::process_filter_node(const proto_msg::FilterNode &node) {

  auto resulting_table = process_node(node.node());
  std::vector<const proto_msg::ExprNode *> nodes;

  for (int i = 0; i < node.exprs_size(); i++) {
    const auto &expr_node = node.exprs(i);
    nodes.push_back(&expr_node);
  }

  if (!extra_str_dict)
    extra_str_dict = std::make_unique<NaiveDynamicStringDictionary>();

  //  ExprListProcessor(*resulting_table, *vim, nodes, cm, *extra_str_dict)
  //      .execute();

  return std::make_shared<ResultTableFilterIterator>(
      resulting_table, *vim, nodes, cm, *extra_str_dict);
}
std::shared_ptr<ResultTableIterator>
QueryProcessor2::process_extend_node(const proto_msg::ExtendNode &node) {
  auto resulting_table = process_node(node.node());
  if (!extra_str_dict) {
    extra_str_dict = std::make_unique<NaiveDynamicStringDictionary>();
  }

  auto var_pos_mapping = create_var_pos_mapping(*resulting_table);

  auto eval_data = std::make_unique<EvalData>(
      *vim, cm, std::move(var_pos_mapping), *extra_str_dict);
  std::vector<std::unique_ptr<VarLazyBinding>> var_bindings;
  for (int i = 0; i < node.assignments_size(); i++) {
    const auto &assignment_node = node.assignments(i);
    const auto &var_value = assignment_node.var().term_value();
    vim->assign_index_if_not_found(var_value);
    auto var_id = vim->var_indexes[var_value];
    // resulting_table->headers.push_back(new_header);
    auto expr_eval =
        ExprEval::create_eval_node(*eval_data, assignment_node.expr());
    expr_eval->init();
    var_bindings.push_back(
        std::make_unique<VarLazyBinding>(var_id, std::move(expr_eval)));
  }
  return std::make_shared<ResultTableIteratorExtend>(std::move(resulting_table),
                                                     std::move(eval_data),
                                                     std::move(var_bindings));
}
std::shared_ptr<ResultTableIterator>
QueryProcessor2::process_minus_node(const proto_msg::MinusNode &node) {
  auto left_it = process_node(node.left_node());
  auto right_it = process_node(node.right_node());
  std::set<unsigned long> left_headers_set(left_it->get_headers().begin(),
                                           left_it->get_headers().end());
  // std::set<unsigned long> right_headers(right_it->get_headers().begin(),
  // right_it->get_headers().end());

  for (auto rh : right_it->get_headers()) {
    // right headers have to be a subset of left headers
    if (left_headers_set.find(rh) == left_headers_set.end()) {
      return left_it;
    }
  }

  return std::make_shared<ResultTableIteratorMinus>(left_it, *right_it);
}
std::shared_ptr<ResultTableIterator>
QueryProcessor2::process_sequence_node(const proto_msg::SequenceNode &node) {
  auto result_it = process_node(node.nodes(0));
  for (int i = 1; i < node.nodes_size(); i++) {
    auto current_it = process_node(node.nodes(i));
    result_it = InnerJoinProcessor2(result_it, current_it).execute_it();
  }
  return result_it;
}

std::shared_ptr<ResultTableIterator>
QueryProcessor2::process_slice_node(const proto_msg::SliceNode &node) {
  auto result_it = process_node(node.node());
  const unsigned long start = node.start();
  const unsigned long length = node.length();

  return std::make_shared<ResultTableIteratorSlice>(std::move(result_it), start,
                                                    length);
}
QueryProcessor2::QueryProcessor2(
    const PredicatesCacheManager &cache_manager,
    std::unique_ptr<VarIndexManager> &&vim,
    std::unique_ptr<NaiveDynamicStringDictionary> &&extra_str_dict)
    : cm(cache_manager), vim(std::move(vim)),
      extra_str_dict(std::move(extra_str_dict)) {}

std::shared_ptr<ResultTableIterator>
QueryProcessor2::process_order_node(const proto_msg::OrderNode &node) {
  auto result_it = process_node(node.node());
  auto var_pos_mapping = create_var_pos_mapping(*result_it);

  EvalData eval_data(*vim, cm, std::move(var_pos_mapping), *extra_str_dict);

  return std::make_shared<ResultTableIteratorFromMaterialized>(
      OrderNodeProcessor(result_it->materialize(), node, eval_data).execute());
}

std::shared_ptr<std::unordered_map<std::string, unsigned long>>
QueryProcessor2::create_var_pos_mapping(ResultTableIterator &input_it) {
  auto var_pos_mapping =
      std::make_shared<std::unordered_map<std::string, unsigned long>>();
  auto rev_map = vim->reverse();
  auto &headers = input_it.get_headers();
  for (unsigned long i = 0; i < static_cast<unsigned long>(headers.size());
       i++) {
    auto header = headers[i];
    (*var_pos_mapping)[rev_map[header]] = i;
  }
  return var_pos_mapping;
}
std::set<unsigned long>
QueryProcessor2::get_var_ids(const std::vector<std::string> &vars_vector) {
  std::set<unsigned long> result;
  for (auto &v : vars_vector) {
    result.insert(vim->var_indexes[v]);
  }
  return result;
}
