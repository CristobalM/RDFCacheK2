//
// Created by cristobal on 5/18/21.
//

#include "ExistsEval.hpp"
#include "ExprEval.hpp"
#include <query_processing/QueryProcessor.hpp>
void ExistsEval::init() {
  ExprEval::init();
  was_calculated_constant = false;
  is_constant = false;
}
std::shared_ptr<TermResource>
ExistsEval::eval_resource(const ExprEval::row_t &row) {
  return generate_from_eval_boolean(row);
}
bool ExistsEval::eval_boolean(const ExprEval::row_t &row) {
  has_constant_subtree();
  const auto &query_tree = expr_node.function_node().exprs(0).pattern_node();
  if (!is_constant) {
    auto next_vim = std::make_shared<VarIndexManager>(eval_data.vim);
    auto var_binding_qproc =
        bind_row_vars_next_eval_data(*eval_data.extra_dict, row);
    auto qproc = QueryProcessor(eval_data.cm, std::move(next_vim),
                                eval_data.extra_dict, eval_data.time_control);
    auto rit = qproc.run_query(query_tree, std::move(var_binding_qproc));
    return rit.get_it().has_next();
  } else {
    auto rit = QueryProcessor(eval_data.cm, eval_data.time_control)
                   .run_query(query_tree);
    return rit.get_it().has_next();
  }
}
void ExistsEval::validate() {
  ExprEval::validate();
  assert_fun_size(1);
  assert_is_pattern_node(expr_node.function_node().exprs(0));
}
bool ExistsEval::has_constant_subtree() {
  if (was_calculated_constant)
    return is_constant;
  auto inside_vars = gather_inside_variables();
  auto outside_vars = gather_outside_variables();
  std::set<std::string> intersection;
  std::set_intersection(inside_vars.begin(), inside_vars.end(),
                        outside_vars.begin(), outside_vars.end(),
                        std::inserter(intersection, intersection.begin()));
  is_constant = intersection.empty();
  was_calculated_constant = true;
  return is_constant;
}
std::set<std::string> ExistsEval::gather_inside_variables() const {
  std::set<std::string> vars;
  explore_node_for_vars(expr_node.function_node().exprs(0).pattern_node(),
                        vars);
  return vars;
}
std::set<std::string> ExistsEval::gather_outside_variables() const {
  std::set<std::string> vars;
  for (const auto &pair_var_value : eval_data.vim.var_indexes) {
    vars.insert(pair_var_value.first);
  }
  return vars;
}
void ExistsEval::explore_node_for_vars(const proto_msg::SparqlNode &node,
                                       std::set<std::string> &set) const {
  switch (node.node_case()) {

  case proto_msg::SparqlNode::kProjectNode:
    explore_node_for_vars_project_node(node.project_node(), set);
    break;
  case proto_msg::SparqlNode::kLeftJoinNode:
    explore_node_for_vars_left_join_node(node.left_join_node(), set);
    break;
  case proto_msg::SparqlNode::kBgpNode:
    explore_node_for_vars_bgp_node(node.bgp_node(), set);
    break;
  case proto_msg::SparqlNode::kTripleNode:
    explore_term_for_vars(node.triple_node().subject(), set);
    explore_term_for_vars(node.triple_node().predicate(), set);
    explore_term_for_vars(node.triple_node().object(), set);
    break;
  case proto_msg::SparqlNode::kExprNode:
    explore_expr_for_vars(node.expr_node(), set);
    break;
  case proto_msg::SparqlNode::kUnionNode:
    explore_union_for_vars(node.union_node(), set);
    break;
  case proto_msg::SparqlNode::kDistinctNode:
    explore_node_for_vars(node.distinct_node().sub_node(), set);
    break;
  case proto_msg::SparqlNode::kOptionalNode:
    explore_node_for_vars(node.optional_node().left_node(), set);
    explore_node_for_vars(node.optional_node().right_node(), set);
    break;
  case proto_msg::SparqlNode::kMinusNode:
    explore_node_for_vars(node.minus_node().left_node(), set);
    explore_node_for_vars(node.minus_node().right_node(), set);
    break;
  case proto_msg::SparqlNode::kFilterNode:
    explore_filter_node_for_vars(node.filter_node(), set);
    break;
  case proto_msg::SparqlNode::kExtendNode:
    explore_extend_node_for_vars(node.extend_node(), set);
    break;
  case proto_msg::SparqlNode::kSequenceNode:
    explore_sequence_node_for_vars(node.sequence_node(), set);
    break;
  case proto_msg::SparqlNode::kSliceNode:
    explore_node_for_vars(node.slice_node().node(), set);
    break;
  default:
    return;
  }
}
void ExistsEval::explore_node_for_vars_project_node(
    const proto_msg::ProjectNode &node, std::set<std::string> &set) const {
  for (int i = 0; i < node.vars_size(); i++) {
    set.insert(node.vars(i));
  }
  explore_node_for_vars(node.sub_op(), set);
}
void ExistsEval::explore_node_for_vars_left_join_node(
    const proto_msg::LeftJoinNode &node, std::set<std::string> &set) const {
  explore_node_for_vars(node.left_node(), set);
  explore_node_for_vars(node.right_node(), set);
}
void ExistsEval::explore_node_for_vars_bgp_node(
    const proto_msg::BGPNode &node, std::set<std::string> &set) const {
  for (int i = 0; i < node.triple_size(); i++) {
    const auto &triple = node.triple(i);
    explore_term_for_vars(triple.subject(), set);
    explore_term_for_vars(triple.predicate(), set);
    explore_term_for_vars(triple.object(), set);
  }
}
void ExistsEval::explore_term_for_vars(const proto_msg::RDFTerm &term,
                                       std::set<std::string> &set) const {
  if (term.term_type() == proto_msg::TermType::VARIABLE) {
    set.insert(term.term_value());
  }
}
void ExistsEval::explore_expr_for_vars(const proto_msg::ExprNode &node,
                                       std::set<std::string> &set) const {
  switch (node.expr_case()) {
  case proto_msg::ExprNode::kFunctionNode:
    explore_fun_expr_for_vars(node.function_node(), set);
    break;
  case proto_msg::ExprNode::kTermNode:
    explore_term_for_vars(node.term_node(), set);
    break;
  case proto_msg::ExprNode::kPatternNode:
    explore_node_for_vars(node.pattern_node(), set);
    break;
  default:
    return;
  }
}
void ExistsEval::explore_fun_expr_for_vars(
    const proto_msg::FunctionNode &fun_node, std::set<std::string> &set) const {
  for (int i = 0; i < fun_node.exprs_size(); i++) {
    explore_expr_for_vars(fun_node.exprs(i), set);
  }
}

void ExistsEval::explore_union_for_vars(const proto_msg::UnionNode &node,
                                        std::set<std::string> &set) const {
  for (int i = 0; i < node.nodes_list_size(); i++) {
    explore_node_for_vars(node.nodes_list(i), set);
  }
}
void ExistsEval::explore_filter_node_for_vars(
    const proto_msg::FilterNode &node, std::set<std::string> &set) const {
  explore_node_for_vars(node.node(), set);
  for (int i = 0; i < node.exprs_size(); i++) {
    explore_expr_for_vars(node.exprs(i), set);
  }
}

void ExistsEval::explore_extend_node_for_vars(
    const proto_msg::ExtendNode &node, std::set<std::string> &set) const {
  explore_node_for_vars(node.node(), set);
  for (int i = 0; i < node.assignments_size(); i++) {
    explore_term_for_vars(node.assignments(i).var(), set);
    explore_expr_for_vars(node.assignments(i).expr(), set);
  }
}
void ExistsEval::explore_sequence_node_for_vars(
    const proto_msg::SequenceNode &node, std::set<std::string> &set) const {
  for (int i = 0; i < node.nodes_size(); i++) {
    explore_node_for_vars(node.nodes(i), set);
  }
}
std::shared_ptr<VarBindingQProc> ExistsEval::bind_row_vars_next_eval_data(
    NaiveDynamicStringDictionary &dictionary, const row_t &row) {
  auto result = std::make_shared<VarBindingQProc>();
  for (const auto &var_pair : eval_data.vim.var_indexes) {
    const auto &var_name = var_pair.first;
    const auto var_id = var_pair.second;
    RDFResource resource;
    if (this->eval_data.var_pos_mapping->find(var_name) ==
        this->eval_data.var_pos_mapping->end())
      continue;
    auto pos = this->eval_data.var_pos_mapping->at(var_name);
    auto value_id = row[pos];
    auto last_cache_id = this->eval_data.cm->get_last_id();
    if (value_id > last_cache_id)
      resource = this->eval_data.extra_dict->extract_resource(value_id -
                                                              last_cache_id);
    else
      resource = this->eval_data.cm->extract_resource(value_id);

    dictionary.add_resource(resource);
    result->bind(var_id, value_id);
  }
  return result;
}
