//
// Created by cristobal on 25-07-21.
//

#include "ProtoGatherVars.hpp"
void ProtoGatherVars::get_vars_from_node_rec(
    const proto_msg::SparqlNode &node, std::set<unsigned long> &table_vars,
    std::set<unsigned long> &ref_vars, bool mark_table_vars_as_ref,
    VarIndexManager &vim) {

  switch (node.node_case()) {

  case proto_msg::SparqlNode::kProjectNode: {
    for (int i = 0; i < node.project_node().vars_size(); i++) {
      cond_add_var(node.project_node().vars(i), table_vars, ref_vars,
                   mark_table_vars_as_ref, vim);
    }
    get_vars_from_node_rec(node.project_node().sub_op(), table_vars, ref_vars,
                           true, vim);
  } break;
  case proto_msg::SparqlNode::kLeftJoinNode: {
    get_vars_from_node_rec(node.left_join_node().left_node(), table_vars,
                           ref_vars, mark_table_vars_as_ref, vim);
    get_vars_from_node_rec(node.left_join_node().right_node(), table_vars,
                           ref_vars, mark_table_vars_as_ref, vim);
    for (int i = 0; i < node.left_join_node().expr_list_size(); i++) {
      get_vars_from_expr_rec(node.left_join_node().expr_list(i), table_vars,
                             ref_vars, vim);
    }
  } break;

  case proto_msg::SparqlNode::kBgpNode: {
    for (int i = 0; i < node.bgp_node().triple_size(); i++) {
      add_vars_from_triple(node.bgp_node().triple(i), table_vars, ref_vars,
                           mark_table_vars_as_ref, vim);
    }
  } break;
  case proto_msg::SparqlNode::kUnionNode: {
    for (int i = 0; i < node.union_node().nodes_list_size(); i++) {
      get_vars_from_node_rec(node.union_node().nodes_list(i), table_vars,
                             ref_vars, mark_table_vars_as_ref, vim);
    }
  } break;
  case proto_msg::SparqlNode::kDistinctNode: {
    get_vars_from_node_rec(node.distinct_node().sub_node(), table_vars,
                           ref_vars, mark_table_vars_as_ref, vim);
  } break;
  case proto_msg::SparqlNode::kOptionalNode: {
    get_vars_from_node_rec(node.optional_node().left_node(), table_vars,
                           ref_vars, mark_table_vars_as_ref, vim);
    get_vars_from_node_rec(node.optional_node().right_node(), table_vars,
                           ref_vars, mark_table_vars_as_ref, vim);
  } break;
  case proto_msg::SparqlNode::kMinusNode: {
    get_vars_from_node_rec(node.minus_node().left_node(), table_vars, ref_vars,
                           mark_table_vars_as_ref, vim);
    get_vars_from_node_rec(node.minus_node().right_node(), table_vars, ref_vars,
                           mark_table_vars_as_ref, vim);
  } break;
  case proto_msg::SparqlNode::kFilterNode: {
    get_vars_from_node_rec(node.filter_node().node(), table_vars, ref_vars,
                           mark_table_vars_as_ref, vim);
    for (int i = 0; i < node.filter_node().exprs_size(); i++)
      get_vars_from_expr_rec(node.filter_node().exprs(i), table_vars, ref_vars,
                             vim);
  } break;
  case proto_msg::SparqlNode::kExtendNode: {
    get_vars_from_node_rec(node.extend_node().node(), table_vars, ref_vars,
                           mark_table_vars_as_ref, vim);
    for (int i = 0; i < node.extend_node().assignments_size(); i++) {
      add_vars_from_assignment(node.extend_node().assignments(i), table_vars,
                               ref_vars, mark_table_vars_as_ref, vim);
      get_vars_from_expr_rec(node.extend_node().assignments(i).expr(),
                             table_vars, ref_vars, vim);
    }
  } break;
  case proto_msg::SparqlNode::kSequenceNode: {
    const auto &seq_node = node.sequence_node();
    for (int i = 0; i < seq_node.nodes_size(); i++) {
      get_vars_from_node_rec(seq_node.nodes(i), table_vars, ref_vars,
                             mark_table_vars_as_ref, vim);
    }
  } break;
  case proto_msg::SparqlNode::kSliceNode: {
    const auto &slice_node = node.slice_node();
    get_vars_from_node_rec(slice_node.node(), table_vars, ref_vars,
                           mark_table_vars_as_ref, vim);
  } break;
  case proto_msg::SparqlNode::kOrderNode: {
    const auto &order_node = node.order_node();
    get_vars_from_node_rec(order_node.node(), table_vars, ref_vars,
                           mark_table_vars_as_ref, vim);
    for (int i = 0; i < order_node.sort_conditions_size(); i++) {
      const auto &sort_condition = order_node.sort_conditions(i);
      get_vars_from_expr_rec(sort_condition.expr(), table_vars, ref_vars, vim);
    }
  } break;
  case proto_msg::SparqlNode::kTableNode: {
    const auto &table_node = node.table_node();
    for (int i = 0; i < table_node.vars_size(); i++) {
      cond_add_var(table_node.vars(i), table_vars, ref_vars,
                   mark_table_vars_as_ref, vim);
    }
  } break;

  default:
    break;
  }
}
GatheredVars
ProtoGatherVars::get_vars_from_node(VarIndexManager &vim,
                                    const proto_msg::SparqlNode &node) {

  std::set<unsigned long> table_vars;
  std::set<unsigned long> ref_vars;
  get_vars_from_node_rec(node, table_vars, ref_vars, false, vim);
  return {std::move(table_vars), std::move(ref_vars)};
}
void ProtoGatherVars::add_vars_from_triple(
    const proto_msg::TripleNode &triple_node,
    std::set<unsigned long> &table_vars, std::set<unsigned long> &ref_vars,
    bool mark_table_vars_as_ref, VarIndexManager &vim) {

  if (triple_node.subject().term_type() == proto_msg::TermType::VARIABLE)
    cond_add_var(triple_node.subject().term_value(), table_vars, ref_vars,
                 mark_table_vars_as_ref, vim);
  if (triple_node.predicate().term_type() == proto_msg::TermType::VARIABLE)
    cond_add_var(triple_node.predicate().term_value(), table_vars, ref_vars,
                 mark_table_vars_as_ref, vim);
  if (triple_node.object().term_type() == proto_msg::TermType::VARIABLE)
    cond_add_var(triple_node.object().term_value(), table_vars, ref_vars,
                 mark_table_vars_as_ref, vim);
}
void ProtoGatherVars::get_vars_from_expr_rec(
    const proto_msg::ExprNode &expr_node, std::set<unsigned long> &table_vars,
    std::set<unsigned long> &ref_vars, VarIndexManager &vim) {
  {

    switch (expr_node.expr_case()) {
    case proto_msg::ExprNode::kFunctionNode: {
      const auto &fun_node = expr_node.function_node();
      for (int i = 0; i < fun_node.exprs_size(); i++) {
        get_vars_from_expr_rec(fun_node.exprs(i), table_vars, ref_vars, vim);
      }
    } break;
    case proto_msg::ExprNode::kTermNode: {
      const auto &term_node = expr_node.term_node();
      if (term_node.term_type() == proto_msg::TermType::VARIABLE) {
        ref_vars.insert(vim.assign_index_if_not_found(term_node.term_value()));
      }
    } break;
    case proto_msg::ExprNode::kPatternNode: {
      const auto &pattern_node = expr_node.pattern_node();
      get_vars_from_node_rec(pattern_node, table_vars, ref_vars, true, vim);
    } break;
    case proto_msg::ExprNode::EXPR_NOT_SET:
      break;
    }
  }
}
void ProtoGatherVars::add_vars_from_assignment(
    const proto_msg::AssignmentNode &assignment_node,
    std::set<unsigned long> &table_vars, std::set<unsigned long> &ref_vars,
    bool mark_table_vars_as_ref, VarIndexManager &vim) {

  cond_add_var(assignment_node.var().term_value(), table_vars, ref_vars,
               mark_table_vars_as_ref, vim);
  get_vars_from_expr_rec(assignment_node.expr(), table_vars, ref_vars, vim);
}
void ProtoGatherVars::cond_add_var(const std::string &var_str,
                                   std::set<unsigned long> &table_vars,
                                   std::set<unsigned long> &ref_vars,
                                   bool mark_table_vars_as_ref,
                                   VarIndexManager &vim) {

  if (!mark_table_vars_as_ref)
    table_vars.insert(vim.assign_index_if_not_found(var_str));
  else
    ref_vars.insert(vim.assign_index_if_not_found(var_str));
}
