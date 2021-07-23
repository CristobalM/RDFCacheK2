//
// Created by cristobal on 7/21/21.
//

#include "SequenceProcessor.hpp"
#include "ResultTableIteratorCrossProduct.hpp"
#include "ResultTableIteratorEmpty.hpp"
#include "ResultTableIteratorJoinSeqBinding.hpp"
#include "VarBindingQProc.hpp"

std::shared_ptr<ResultTableIterator> SequenceProcessor::execute_it() {
  std::vector<std::shared_ptr<ResultTableIterator>> to_cross_product_iterators;
  for (size_t cc_i = 0; cc_i < cc_positions.size(); cc_i++) {
    auto &cc_pos = cc_positions[cc_i];
    auto &current_sets = cc_sets[cc_i];
    const auto &first_proto_node = sequence_node.nodes(cc_pos[0]);

    // std::set<unsigned long> resulting_vars_set_current;

    auto result_it =
        query_processor->process_node(first_proto_node, var_binding_qproc);

    // resulting_vars_set_current = current_sets[0];
    for (size_t i = 1; i < cc_pos.size(); i++) {
      int pos = cc_pos[i];
      const auto &curr_proto_node = sequence_node.nodes(pos);

      result_it = std::make_shared<ResultTableIteratorJoinSeqBinding>(
          result_it, curr_proto_node, var_binding_qproc, current_sets[i],
          time_control, query_processor->get_cache_manager(),
          query_processor->get_vim_ptr(),
          query_processor->get_extra_str_dict_ptr());
    }
    to_cross_product_iterators.push_back(std::move(result_it));
  }
  if (to_cross_product_iterators.empty()) {
    return std::make_shared<ResultTableIteratorEmpty>(resulting_headers,
                                                      time_control);
  }

  auto result_it = to_cross_product_iterators[0];
  for (size_t i = 1; i < to_cross_product_iterators.size(); i++) {
    auto curr_it = to_cross_product_iterators[i];
    result_it = std::make_shared<ResultTableIteratorCrossProduct>(
        result_it, std::move(curr_it), time_control);
  }
  return result_it;
}

SequenceProcessor::SequenceProcessor(
    const proto_msg::SequenceNode &sequence_node, QProc *query_processor,
    TimeControl &time_control,
    std::shared_ptr<VarBindingQProc> var_binding_qproc)
    : sequence_node(sequence_node), query_processor(query_processor),
      time_control(time_control),
      var_binding_qproc(std::move(var_binding_qproc)) {
  auto vd_graph = VarDependencyGraph(get_vars_sequence());
  auto [cc_positions, cc_sets] =
      vd_graph.get_connected_components_positions_with_sets();
  this->cc_positions = std::move(cc_positions);
  this->cc_sets = std::move(cc_sets);
  resulting_headers = generate_headers(vd_graph);
}

std::vector<VarsCollection> SequenceProcessor::get_vars_sequence() {
  std::vector<VarsCollection> result;
  for (int i = 0; i < sequence_node.nodes_size(); i++) {
    auto var_collection = get_vars_from_node(sequence_node.nodes(i), i);
    result.push_back(std::move(var_collection));
  }
  return result;
}

VarsCollection
SequenceProcessor::get_vars_from_node(const proto_msg::SparqlNode &node,
                                      int position) {
  VarsCollection result(query_processor->get_vim(), position);
  get_vars_from_node_rec(node, result, false);
  return result;
}
void SequenceProcessor::get_vars_from_node_rec(
    const proto_msg::SparqlNode &node, VarsCollection &vars_collection,
    bool mark_table_vars_as_ref) {

  switch (node.node_case()) {

  case proto_msg::SparqlNode::kProjectNode: {
    for (int i = 0; i < node.project_node().vars_size(); i++) {
      cond_add_var(node.project_node().vars(i), vars_collection,
                   mark_table_vars_as_ref);
    }
    get_vars_from_node_rec(node.project_node().sub_op(), vars_collection, true);
  } break;
  case proto_msg::SparqlNode::kLeftJoinNode: {
    get_vars_from_node_rec(node.left_join_node().left_node(), vars_collection,
                           mark_table_vars_as_ref);
    get_vars_from_node_rec(node.left_join_node().right_node(), vars_collection,
                           mark_table_vars_as_ref);
    for (int i = 0; i < node.left_join_node().expr_list_size(); i++) {
      get_vars_from_expr_rec(node.left_join_node().expr_list(i),
                             vars_collection);
    }
  } break;

  case proto_msg::SparqlNode::kBgpNode: {
    for (int i = 0; i < node.bgp_node().triple_size(); i++) {
      add_vars_from_triple(node.bgp_node().triple(i), vars_collection,
                           mark_table_vars_as_ref);
    }
  } break;
  case proto_msg::SparqlNode::kUnionNode: {
    for (int i = 0; i < node.union_node().nodes_list_size(); i++) {
      get_vars_from_node_rec(node.union_node().nodes_list(i), vars_collection,
                             mark_table_vars_as_ref);
    }
  } break;
  case proto_msg::SparqlNode::kDistinctNode: {
    get_vars_from_node_rec(node.distinct_node().sub_node(), vars_collection,
                           mark_table_vars_as_ref);
  } break;
  case proto_msg::SparqlNode::kOptionalNode: {
    get_vars_from_node_rec(node.optional_node().left_node(), vars_collection,
                           mark_table_vars_as_ref);
    get_vars_from_node_rec(node.optional_node().right_node(), vars_collection,
                           mark_table_vars_as_ref);
  } break;
  case proto_msg::SparqlNode::kMinusNode: {
    get_vars_from_node_rec(node.minus_node().left_node(), vars_collection,
                           mark_table_vars_as_ref);
    get_vars_from_node_rec(node.minus_node().right_node(), vars_collection,
                           mark_table_vars_as_ref);
  } break;
  case proto_msg::SparqlNode::kFilterNode: {
    get_vars_from_node_rec(node.filter_node().node(), vars_collection,
                           mark_table_vars_as_ref);
    for (int i = 0; i < node.filter_node().exprs_size(); i++)
      get_vars_from_expr_rec(node.filter_node().exprs(i), vars_collection);
  } break;
  case proto_msg::SparqlNode::kExtendNode: {
    get_vars_from_node_rec(node.extend_node().node(), vars_collection,
                           mark_table_vars_as_ref);
    for (int i = 0; i < node.extend_node().assignments_size(); i++) {
      add_vars_from_assignment(node.extend_node().assignments(i),
                               vars_collection, mark_table_vars_as_ref);
      get_vars_from_expr_rec(node.extend_node().assignments(i).expr(),
                             vars_collection);
    }
  } break;
  case proto_msg::SparqlNode::kSequenceNode: {
    const auto &seq_node = node.sequence_node();
    for (int i = 0; i < seq_node.nodes_size(); i++) {
      get_vars_from_node_rec(seq_node.nodes(i), vars_collection,
                             mark_table_vars_as_ref);
    }
  } break;
  case proto_msg::SparqlNode::kSliceNode: {
    const auto &slice_node = node.slice_node();
    get_vars_from_node_rec(slice_node.node(), vars_collection,
                           mark_table_vars_as_ref);
  } break;
  case proto_msg::SparqlNode::kOrderNode: {
    const auto &order_node = node.order_node();
    get_vars_from_node_rec(order_node.node(), vars_collection,
                           mark_table_vars_as_ref);
    for (int i = 0; i < order_node.sort_conditions_size(); i++) {
      const auto &sort_condition = order_node.sort_conditions(i);
      get_vars_from_expr_rec(sort_condition.expr(), vars_collection);
    }
  } break;
  case proto_msg::SparqlNode::kTableNode: {
    const auto &table_node = node.table_node();
    for (int i = 0; i < table_node.vars_size(); i++) {
      cond_add_var(table_node.vars(i), vars_collection, mark_table_vars_as_ref);
    }
  } break;

  default:
    break;
  }
}
void SequenceProcessor::add_vars_from_triple(
    const proto_msg::TripleNode &triple_node, VarsCollection &vars_collection,
    bool mark_table_vars_as_ref) {
  if (triple_node.subject().term_type() == proto_msg::TermType::VARIABLE)
    cond_add_var(triple_node.subject().term_value(), vars_collection,
                 mark_table_vars_as_ref);
  if (triple_node.predicate().term_type() == proto_msg::TermType::VARIABLE)
    cond_add_var(triple_node.predicate().term_value(), vars_collection,
                 mark_table_vars_as_ref);
  if (triple_node.object().term_type() == proto_msg::TermType::VARIABLE)
    cond_add_var(triple_node.object().term_value(), vars_collection,
                 mark_table_vars_as_ref);
}

void SequenceProcessor::get_vars_from_expr_rec(
    const proto_msg::ExprNode &expr_node, VarsCollection &vars_collection) {

  switch (expr_node.expr_case()) {
  case proto_msg::ExprNode::kFunctionNode: {
    const auto &fun_node = expr_node.function_node();
    for (int i = 0; i < fun_node.exprs_size(); i++) {
      get_vars_from_expr_rec(fun_node.exprs(i), vars_collection);
    }
  } break;
  case proto_msg::ExprNode::kTermNode: {
    const auto &term_node = expr_node.term_node();
    if (term_node.term_type() == proto_msg::TermType::VARIABLE) {
      vars_collection.add_reference_var(term_node.term_value());
    }
  } break;
  case proto_msg::ExprNode::kPatternNode: {
    const auto &pattern_node = expr_node.pattern_node();
    get_vars_from_node_rec(pattern_node, vars_collection, true);
  } break;
  case proto_msg::ExprNode::EXPR_NOT_SET:
    break;
  }
}
void SequenceProcessor::add_vars_from_assignment(
    const proto_msg::AssignmentNode &assignment_node,
    VarsCollection &vars_collection, bool mark_table_vars_as_ref) {
  cond_add_var(assignment_node.var().term_value(), vars_collection,
               mark_table_vars_as_ref);
  get_vars_from_expr_rec(assignment_node.expr(), vars_collection);
}
void SequenceProcessor::cond_add_var(const std::string &var_str,
                                     VarsCollection &vars_collection,
                                     bool mark_table_vars_as_ref) {
  if (!mark_table_vars_as_ref)
    vars_collection.add_table_var(var_str);
  else
    vars_collection.add_reference_var(var_str);
}
std::vector<unsigned long>
SequenceProcessor::generate_headers(VarDependencyGraph &vd_graph) {
  std::set<unsigned long> result_set;
  for (const auto &vdg_node : vd_graph.get_nodes()) {
    const auto &var_collection = vdg_node->get_collection();
    const auto &table_vars = var_collection.get_table_vars_c();
    for (auto var : table_vars) {
      result_set.insert(var);
    }
  }
  return std::vector<unsigned long>(result_set.begin(), result_set.end());
}
