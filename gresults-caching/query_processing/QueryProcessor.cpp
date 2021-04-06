

//
// Created by Cristobal Miranda, 2020
//

#include <chrono>
#include <set>
#include <sstream>
#include <stdexcept>
#include <string>
#include <unordered_map>
#include <unordered_set>
#include <utility>

#include "BGPProcessor.hpp"
#include "QueryProcessor.hpp"
#include "Term.hpp"
#include "UnionProcessor.hpp"
#include "VarIndexManager.hpp"

QueryProcessor::QueryProcessor(const PredicatesCacheManager &cache_manager)
    : cache_manager(cache_manager) {}

// prototypes

std::shared_ptr<ResultTable>
process_join_node(const proto_msg::SparqlNode &join_node,
                  const PredicatesCacheManager &cm, VarIndexManager &vim);

void process_expr_node(const proto_msg::ExprNode &,
                       std::shared_ptr<ResultTable> &,
                       PredicatesCacheManager &);

std::shared_ptr<ResultTable>
process_left_join_node(const proto_msg::LeftJoinNode &left_join_node,
                       const PredicatesCacheManager &cm, VarIndexManager &vim);

std::shared_ptr<ResultTable>
process_project_node(const proto_msg::ProjectNode &project_node,
                     const PredicatesCacheManager &cm, VarIndexManager &vim);

std::shared_ptr<ResultTable>
process_union_node(const proto_msg::UnionNode &union_node,
                   const PredicatesCacheManager &cm, VarIndexManager &vim);

void process_triple_node(const proto_msg::TripleNode &,
                         std::shared_ptr<ResultTable> &,
                         PredicatesCacheManager &);

void remove_extra_vars_from_table(std::shared_ptr<ResultTable> input_table,
                                  const std::vector<std::string> &vars,
                                  VarIndexManager &vim);
// definitions

std::shared_ptr<ResultTable>
process_join_node(const proto_msg::SparqlNode &join_node,
                  const PredicatesCacheManager &cm, VarIndexManager &vim) {

  std::shared_ptr<ResultTable> result_table;

  switch (join_node.node_case()) {
  case proto_msg::SparqlNode::NodeCase::kBgpNode:
    result_table = BGPProcessor(join_node.bgp_node(), cm, vim).execute();
    break;
  case proto_msg::SparqlNode::NodeCase::kLeftJoinNode:
    result_table = process_left_join_node(join_node.left_join_node(), cm, vim);
    break;
  default:
    throw std::runtime_error("Unsupported nodetype on process_join_node: " +
                             std::to_string(join_node.node_case()));
  }

  return result_table;
}

void process_expr_node(const proto_msg::ExprNode &,
                       std::shared_ptr<ResultTable> &,
                       PredicatesCacheManager &) {}

std::shared_ptr<ResultTable>
process_left_join_node(const proto_msg::LeftJoinNode &left_join_node,
                       const PredicatesCacheManager &cm, VarIndexManager &vim) {
  // TODO: FIX
  auto left_result = process_join_node(left_join_node.left_node(), cm, vim);
  const auto right_type = left_join_node.right_node().node_case();
  if (right_type == proto_msg::SparqlNode::NodeCase::kBgpNode) {
    const auto &bgp_node = left_join_node.right_node().bgp_node();
    for (int i = 0; i < bgp_node.triple_size(); i++) {
      const auto &current_triple = bgp_node.triple(i);
      /*
      K2TreeMixed *current_k2tree =
          k2trees_map[current_triple.subject().term_value()];
          */
      auto subject_is_var =
          current_triple.subject().term_type() == proto_msg::TermType::VARIABLE;
      auto object_is_var =
          current_triple.object().term_type() == proto_msg::TermType::VARIABLE;
      if (subject_is_var && object_is_var) {

      } else if (subject_is_var) {

      } else if (object_is_var) {

      } else {
        throw std::runtime_error("found triple without variables: " +
                                 current_triple.subject().term_value() + ", " +
                                 current_triple.predicate().term_value() +
                                 ", " + current_triple.object().term_value());
      }
    }
  } else if (right_type == proto_msg::SparqlNode::NodeCase::kLeftJoinNode) {

  } else {
    throw std::runtime_error(
        "process_left_join_node: unexpected right node type: " +
        std::to_string(right_type));
  }

  return left_result;
}

void remove_extra_vars_from_table(std::shared_ptr<ResultTable> input_table,
                                  const std::vector<std::string> &vars,
                                  VarIndexManager &vim) {

  // std::unordered_set<std::string> vars_set(vars.begin(), vars.end());
  std::unordered_set<unsigned long> vars_set;
  std::unordered_set<unsigned long> columns_to_erase;
  for (auto var : vars) {
    vars_set.insert(vim.var_indexes[var]);
  }

  std::unordered_set<unsigned long> headers_set;
  for (auto var_value : input_table->headers) {
    headers_set.insert(var_value);
  }

  if (vars_set == headers_set)
    return;

  std::vector<unsigned long> next_headers;
  for (size_t h_i = 0; h_i < input_table->headers.size(); h_i++) {
    auto header_value = input_table->headers[h_i];
    if (vars_set.find(header_value) == vars_set.end()) {
      columns_to_erase.insert(h_i);
    } else {
      next_headers.push_back(header_value);
    }
  }

  input_table->headers = std::move(next_headers);

  for (auto &row : input_table->data) {
    std::vector<unsigned long> next_row;
    for (size_t i = 0; i < row.size(); i++) {
      if (columns_to_erase.find(i) == columns_to_erase.end()) {
        next_row.push_back(row[i]);
      }
    }
    row = std::move(next_row);
  }
}

void remove_repeated_rows(std::shared_ptr<ResultTable> input_table) {
  auto left_it = input_table->data.begin();

  for (auto it = std::next(left_it); it != input_table->data.end();) {
    if (*it == *left_it) {
      auto next_it = std::next(it);
      input_table->data.erase(it);
      it = next_it;
    } else {
      left_it = it;
      it++;
    }
  }
}

std::shared_ptr<ResultTable>
process_project_node(const proto_msg::ProjectNode &project_node,
                     const PredicatesCacheManager &cm, VarIndexManager &vim) {
  std::vector<std::string> vars;
  for (int i = 0; i < project_node.vars_size(); i++) {
    vars.push_back(project_node.vars(i));
  }

  for (const auto &var : vars) {
    vim.assign_index_if_not_found(var);
  }

  std::shared_ptr<ResultTable> result_table;

  switch (project_node.sub_op().node_case()) {
  case proto_msg::SparqlNode::NodeCase::kBgpNode:
    result_table =
        BGPProcessor(project_node.sub_op().bgp_node(), cm, vim).execute();
    break;
  case proto_msg::SparqlNode::NodeCase::kLeftJoinNode:
    result_table =
        process_left_join_node(project_node.sub_op().left_join_node(), cm, vim);
    break;
  case proto_msg::SparqlNode::NodeCase::kUnionNode:
    result_table =
        process_union_node(project_node.sub_op().union_node(), cm, vim);
    break;
  default:
    throw std::runtime_error("Unsupported nodetype on process_project_node: " +
                             std::to_string(project_node.sub_op().node_case()));
  }

  remove_extra_vars_from_table(result_table, vars, vim);
  // remove_repeated_rows(result_table);
  return result_table;
}

void process_triple_node(const proto_msg::TripleNode &,
                         std::shared_ptr<ResultTable> &,
                         PredicatesCacheManager &) {}

std::shared_ptr<ResultTable> process_node(const proto_msg::SparqlNode &node,
                                          const PredicatesCacheManager &cm,
                                          VarIndexManager &vim) {
  if (node.node_case() != proto_msg::SparqlNode::NodeCase::kProjectNode)
    throw std::runtime_error("(process_node) Expected ProjectNode, but got: " +
                             std::to_string(node.node_case()));
  return process_project_node(node.project_node(), cm, vim);
}

QueryResult QueryProcessor::run_query(proto_msg::SparqlTree const &query_tree) {
  VarIndexManager vim;
  auto result = process_node(query_tree.root(), cache_manager, vim);
  return QueryResult(result, std::move(vim));
}

std::shared_ptr<ResultTable>
process_union_node(const proto_msg::UnionNode &union_node,
                   const PredicatesCacheManager &cm, VarIndexManager &vim) {

  UnionProcessor union_processor(vim);
  for (int i = 0; i < union_node.nodes_list_size(); i++) {
    auto &node = union_node.nodes_list(i);
    auto table = process_node(node, cm, vim);
    union_processor.combine_table(std::move(table));
  }
  return union_processor.get_result();
}
