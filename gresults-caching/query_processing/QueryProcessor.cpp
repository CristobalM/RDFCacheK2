

//
// Created by Cristobal Miranda, 2020
//

#include <chrono>
#include <query_processing/expr/ExprEval.hpp>
#include <set>
#include <sstream>
#include <stdexcept>
#include <string>
#include <unordered_map>
#include <unordered_set>
#include <utility>
#include <utils/Utils.h>

#include "BGPProcessor.hpp"
#include "ExprListProcessor.hpp"
#include "ExprProcessorPersistentData.hpp"
#include "OptionalProcessor.hpp"
#include "QueryProcessor.hpp"
#include "Term.hpp"
#include "UnionProcessor.hpp"
#include "VarIndexManager.hpp"

QueryProcessor::QueryProcessor(const PredicatesCacheManager &cache_manager)
    : cm(cache_manager), vim(std::make_unique<VarIndexManager>()) {}

std::shared_ptr<ResultTable>
QueryProcessor::process_join_node(const proto_msg::SparqlNode &join_node) {

  std::shared_ptr<ResultTable> result_table;

  switch (join_node.node_case()) {
  case proto_msg::SparqlNode::NodeCase::kBgpNode:
    result_table = BGPProcessor(join_node.bgp_node(), cm, *vim).execute();
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

std::shared_ptr<ResultTable> QueryProcessor::process_left_join_node(
    const proto_msg::LeftJoinNode &left_join_node) {
  auto left_table = process_node(left_join_node.left_node());
  auto right_table = process_node(left_join_node.right_node());
  auto resulting_table = OptionalProcessor(left_table, right_table).execute();

  std::vector<const proto_msg::ExprNode *> nodes;

  for (int i = 0; i < left_join_node.expr_list_size(); i++) {
    const auto &expr_node = left_join_node.expr_list(i);
    nodes.push_back(&expr_node);
  }

  if (extra_str_dict)
    extra_str_dict = std::make_unique<NaiveDynamicStringDictionary>();

  ExprListProcessor(*resulting_table, *vim, nodes, cm, *extra_str_dict)
      .execute();

  return resulting_table;
}

void QueryProcessor::remove_extra_vars_from_table(
    std::shared_ptr<ResultTable> input_table,
    const std::vector<std::string> &vars) {

  // std::unordered_set<std::string> vars_set(vars.begin(), vars.end());
  std::unordered_set<unsigned long> vars_set;
  std::unordered_set<unsigned long> columns_to_erase;
  for (const auto &var : vars) {
    vars_set.insert(vim->var_indexes[var]);
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

std::shared_ptr<ResultTable> QueryProcessor::process_project_node(
    const proto_msg::ProjectNode &project_node) {
  std::vector<std::string> vars;
  for (int i = 0; i < project_node.vars_size(); i++) {
    vars.push_back(project_node.vars(i));
  }

  for (const auto &var : vars) {
    vim->assign_index_if_not_found(var);
  }

  auto result_table = process_node(project_node.sub_op());

  remove_extra_vars_from_table(result_table, vars);
  // remove_repeated_rows(result_table);
  return result_table;
}

std::shared_ptr<ResultTable>
QueryProcessor::process_node(const proto_msg::SparqlNode &node) {

  switch (node.node_case()) {
  case proto_msg::SparqlNode::NodeCase::kProjectNode:
    return process_project_node(node.project_node());
  case proto_msg::SparqlNode::NodeCase::kBgpNode:
    return BGPProcessor(node.bgp_node(), cm, *vim).execute();
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
  default:
    throw std::runtime_error("Unsupported nodetype on process_node: " +
                             std::to_string(node.node_case()));
  }
}

QueryResult QueryProcessor::run_query(proto_msg::SparqlTree const &query_tree) {
  auto result = process_node(query_tree.root());
  if (extra_str_dict)
    return QueryResult(result, std::move(vim), std::move(extra_str_dict));
  return QueryResult(result, std::move(vim));
}

std::shared_ptr<ResultTable>
QueryProcessor::process_union_node(const proto_msg::UnionNode &union_node) {
  UnionProcessor union_processor;
  for (int i = 0; i < union_node.nodes_list_size(); i++) {
    auto &node = union_node.nodes_list(i);
    auto table = process_node(node);
    union_processor.combine_table(std::move(table));
  }
  return union_processor.get_result();
}

std::shared_ptr<ResultTable> QueryProcessor::process_distinct_node(
    const proto_msg::DistinctNode &distinct_node) {
  auto table = process_node(distinct_node.sub_node());
  left_to_right_sort(*table);
  remove_repeated_rows(*table);
  return table;
}

std::shared_ptr<ResultTable> QueryProcessor::process_optional_node(
    const proto_msg::OptionalNode &optional_node) {
  auto left_table = process_node(optional_node.left_node());
  auto right_table = process_node(optional_node.right_node());
  return OptionalProcessor(left_table, right_table).execute();
}

void QueryProcessor::remove_repeated_rows(ResultTable &input_table) {
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

void QueryProcessor::left_to_right_sort(ResultTable &input_table) {

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

std::shared_ptr<ResultTable>
QueryProcessor::process_filter_node(const proto_msg::FilterNode &node) {

  auto resulting_table = process_node(node.node());
  std::vector<const proto_msg::ExprNode *> nodes;

  for (int i = 0; i < node.exprs_size(); i++) {
    const auto &expr_node = node.exprs(i);
    nodes.push_back(&expr_node);
  }

  if (!extra_str_dict)
    extra_str_dict = std::make_unique<NaiveDynamicStringDictionary>();

  ExprListProcessor(*resulting_table, *vim, nodes, cm, *extra_str_dict)
      .execute();

  return resulting_table;
}
std::shared_ptr<ResultTable>
QueryProcessor::process_extend_node(const proto_msg::ExtendNode &node) {
  auto resulting_table = process_node(node.node());
  if (!extra_str_dict) {
    extra_str_dict = std::make_unique<NaiveDynamicStringDictionary>();
  }

  std::unordered_map<std::string, unsigned long> var_pos_mapping;
  auto rev_map = vim->reverse();

  for (unsigned long i = 0;
       i < static_cast<unsigned long>(resulting_table->headers.size()); i++) {
    auto header = resulting_table->headers[i];
    var_pos_mapping[rev_map[header]] = i;
  }

  EvalData eval_data(*resulting_table, *vim, cm, var_pos_mapping,
                     *extra_str_dict);
  for (int i = 0; i < node.assignments_size(); i++) {
    const auto &assignment_node = node.assignments(i);
    const auto &var_value = assignment_node.var().term_value();
    vim->assign_index_if_not_found(var_value);
    auto new_header = vim->var_indexes[var_value];
    resulting_table->headers.push_back(new_header);
    auto expr_eval =
        ExprEval::create_eval_node(eval_data, assignment_node.expr());
    expr_eval->init();

    for (auto &row : resulting_table->data) {
      auto resource = expr_eval->produce_resource(row);
      auto concrete_resource = resource->get_resource_clone();
      auto resource_id = cm.get_resource_index(concrete_resource);
      if (resource_id == NORESULT) {
        auto extra_dict_value =
            extra_str_dict->locate_resource(concrete_resource);
        if (extra_dict_value == NORESULT) {
          extra_str_dict->add_resource(concrete_resource);
          extra_dict_value = extra_str_dict->locate_resource(concrete_resource);
        }
        resource_id = extra_dict_value + cm.get_last_id();
      }
      row.push_back(resource_id);
    }
  }

  return resulting_table;
}
