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
  // std::unique_ptr<QueryResultIterator> query_result = nullptr;

  proto_msg::SparqlTree sparql_tree;
  // auto pattern_copy = expr_node.function_node().exprs(0).pattern_node();
  // *sparql_tree.mutable_root() = pattern_copy;
  sparql_tree.mutable_root()->CopyFrom(
      expr_node.function_node().exprs(0).pattern_node());
  if (!is_constant) {
    auto next_vim = std::make_unique<VarIndexManager>(eval_data.vim);
    auto extra_dict =
        std::make_unique<NaiveDynamicStringDictionary>(eval_data.extra_dict);
    auto bound_vars_map = std::make_unique<BoundVarsMap>();

    bind_row_vars_next_eval_data(*extra_dict, *bound_vars_map, row);
    bind_vars_to_sparql_tree(*bound_vars_map, sparql_tree);
    /*
        query_result = std::make_unique<QueryResultIterator>(
            QueryProcessor(eval_data.cm, std::move(next_vim),
                            std::move(extra_dict), eval_data.time_control)
                .run_query(sparql_tree));
        */
    auto qproc = QueryProcessor(eval_data.cm, std::move(next_vim),
                                std::move(extra_dict), eval_data.time_control);
    auto rit = qproc.run_query(sparql_tree);
    return rit.get_it().has_next();
  } else {
    /*
    query_result = std::make_unique<QueryResultIterator>(
        QueryProcessor(eval_data.cm, eval_data.time_control)
            .run_query(sparql_tree));
            */
    auto rit = QueryProcessor(eval_data.cm, eval_data.time_control)
                   .run_query(sparql_tree);
    return rit.get_it().has_next();
  }
  // return query_result->get_it().has_next();
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
void ExistsEval::bind_row_vars_next_eval_data(
    NaiveDynamicStringDictionary &dictionary, BoundVarsMap &bound_vars_map,
    const row_t &row) {

  for (const auto &var_pair : eval_data.vim.var_indexes) {
    const auto &var_name = var_pair.first;
    RDFResource resource;
    if (this->eval_data.var_pos_mapping->find(var_name) ==
        this->eval_data.var_pos_mapping->end())
      continue;
    auto pos = this->eval_data.var_pos_mapping->at(var_name);
    auto value_id = row[pos];
    auto last_cache_id = this->eval_data.cm->get_last_id();
    if (value_id > last_cache_id)
      resource =
          this->eval_data.extra_dict.extract_resource(value_id - last_cache_id);
    else
      resource = this->eval_data.cm->extract_resource(value_id);

    dictionary.add_resource(resource);
    bound_vars_map.bind(var_name, std::move(resource));
  }
}

void ExistsEval::bind_vars_to_sparql_tree(BoundVarsMap &map,
                                          proto_msg::SparqlTree &sparql_tree) {
  bind_vars_to_node(map, *sparql_tree.mutable_root());
}
void ExistsEval::bind_vars_to_node(BoundVarsMap &map,
                                   proto_msg::SparqlNode &node) {
  switch (node.node_case()) {
  case proto_msg::SparqlNode::kProjectNode:
    bind_vars_to_project_node(map, *node.mutable_project_node());
    break;
  case proto_msg::SparqlNode::kLeftJoinNode:
    bind_vars_to_node(map, *node.mutable_left_join_node()->mutable_left_node());
    bind_vars_to_node(map,
                      *node.mutable_left_join_node()->mutable_right_node());
    break;
  case proto_msg::SparqlNode::kBgpNode:
    bind_vars_to_bgp_node(map, *node.mutable_bgp_node());
    break;
  case proto_msg::SparqlNode::kExprNode:
    bind_vars_to_expr_node(map, *node.mutable_expr_node());
    break;
  case proto_msg::SparqlNode::kUnionNode:
    bind_vars_to_union_node(map, *node.mutable_union_node());
    break;
  case proto_msg::SparqlNode::kDistinctNode:
    bind_vars_to_node(map, *node.mutable_distinct_node()->mutable_sub_node());
    break;
  case proto_msg::SparqlNode::kOptionalNode:
    bind_vars_to_node(map, *node.mutable_optional_node()->mutable_left_node());
    bind_vars_to_node(map, *node.mutable_optional_node()->mutable_right_node());
    break;
  case proto_msg::SparqlNode::kMinusNode:
    bind_vars_to_node(map, *node.mutable_minus_node()->mutable_left_node());
    bind_vars_to_node(map, *node.mutable_minus_node()->mutable_right_node());
    break;
  case proto_msg::SparqlNode::kFilterNode:
    bind_vars_to_filter_node(map, *node.mutable_filter_node());
    break;
  case proto_msg::SparqlNode::kExtendNode:
    bind_vars_to_extend_node(map, *node.mutable_extend_node());
    break;
  case proto_msg::SparqlNode::kSequenceNode:
    bind_vars_to_sequence_node(map, *node.mutable_sequence_node());
    break;
  case proto_msg::SparqlNode::kSliceNode:
    bind_vars_to_node(map, *node.mutable_slice_node()->mutable_node());
    break;
  default:
    with_error = true;
  }
}
void ExistsEval::bind_vars_to_project_node(BoundVarsMap &map,
                                           proto_msg::ProjectNode &node) {
  for (int i = 0; i < node.vars_size(); i++) {
    const auto &var = node.vars(i);
    if (map.has_var(var)) {
      replace_vars(map, node);
      break;
    }
  }
  bind_vars_to_node(map, *node.mutable_sub_op());
}
void ExistsEval::replace_vars(BoundVarsMap &map, proto_msg::ProjectNode &node) {
  std::vector<std::string> next_vars;
  for (int i = 0; i < node.vars_size(); i++) {
    const auto &var = node.vars(i);
    if (!map.has_var(var))
      next_vars.push_back(var);
  }
  node.clear_vars();
  for (const auto &var : next_vars) {
    node.mutable_vars()->Add(std::string(var));
  }
}
void ExistsEval::bind_vars_to_bgp_node(BoundVarsMap &map,
                                       proto_msg::BGPNode &bgp_node) {
  for (int i = 0; i < bgp_node.triple_size(); i++) {
    auto &triple = *bgp_node.mutable_triple(i);

    bind_vars_to_term_node(map, *triple.mutable_subject());
    bind_vars_to_term_node(map, *triple.mutable_predicate());
    bind_vars_to_term_node(map, *triple.mutable_object());
  }
}
void ExistsEval::bind_vars_to_term_node(BoundVarsMap &map,
                                        proto_msg::RDFTerm &term) {
  if (term.term_type() != proto_msg::TermType::VARIABLE ||
      !map.has_var(term.term_value()))
    return;
  auto resource = map.get_resource_from(term.term_value());
  switch (resource.resource_type) {
  case RDF_TYPE_IRI:
    term.set_term_type(proto_msg::TermType::IRI);
    break;
  case RDF_TYPE_BLANK:
    term.set_term_type(proto_msg::TermType::BLANK_NODE);
    break;
  case RDF_TYPE_LITERAL:
    term.set_term_type(proto_msg::TermType::LITERAL);
    break;
  default:
    with_error = true;
  }
  term.set_term_value(std::move(resource.value));
}
void ExistsEval::bind_vars_to_expr_node(BoundVarsMap &map,
                                        proto_msg::ExprNode &expr_node) {
  switch (expr_node.expr_case()) {
  case proto_msg::ExprNode::kFunctionNode:
    bind_vars_to_function_expr_node(map, *expr_node.mutable_function_node());
    break;
  case proto_msg::ExprNode::kTermNode:
    bind_vars_to_term_node(map, *expr_node.mutable_term_node());
    break;
  case proto_msg::ExprNode::kPatternNode:
    bind_vars_to_node(map, *expr_node.mutable_pattern_node());
    break;
  default:
    with_error = true;
  }
}
void ExistsEval::bind_vars_to_function_expr_node(
    BoundVarsMap &map, proto_msg::FunctionNode &node) {
  for (int i = 0; i < node.exprs_size(); i++) {
    bind_vars_to_expr_node(map, *node.mutable_exprs(i));
  }
}
void ExistsEval::bind_vars_to_union_node(BoundVarsMap &map,
                                         proto_msg::UnionNode &node) {
  for (int i = 0; i < node.nodes_list_size(); i++) {
    bind_vars_to_node(map, *node.mutable_nodes_list(i));
  }
}
void ExistsEval::bind_vars_to_filter_node(BoundVarsMap &map,
                                          proto_msg::FilterNode &node) {
  for (int i = 0; i < node.exprs_size(); i++) {
    bind_vars_to_expr_node(map, *node.mutable_exprs(i));
  }
  bind_vars_to_node(map, *node.mutable_node());
}
void ExistsEval::bind_vars_to_extend_node(BoundVarsMap &map,
                                          proto_msg::ExtendNode &node) {
  for (int i = 0; i < node.assignments_size(); i++) {
    bind_vars_to_expr_node(map, *node.mutable_assignments(i)->mutable_expr());
  }
  bind_vars_to_node(map, *node.mutable_node());
}
void ExistsEval::bind_vars_to_sequence_node(BoundVarsMap &map,
                                            proto_msg::SequenceNode &node) {
  for (int i = 0; i < node.nodes_size(); i++) {
    bind_vars_to_node(map, *node.mutable_nodes(i));
  }
}
