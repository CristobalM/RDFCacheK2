//
// Created by cristobal on 5/18/21.
//

#ifndef RDFCACHEK2_EXISTSEVAL_HPP
#define RDFCACHEK2_EXISTSEVAL_HPP

#include "ExprEval.hpp"
class ExistsEval : public ExprEval {
  bool was_calculated_constant;
  bool is_constant;

public:
  using ExprEval::ExprEval;
  void init() override;
  std::shared_ptr<TermResource> eval_resource(const row_t &row) override;
  bool eval_boolean(const row_t &row) override;
  void validate() override;

private:
  bool has_constant_subtree() override;
  std::set<std::string> gather_inside_variables() const;
  std::set<std::string> gather_outside_variables() const;
  void explore_node_for_vars(const proto_msg::SparqlNode &node,
                             std::set<std::string> &set) const;
  void explore_node_for_vars_project_node(const proto_msg::ProjectNode &node,
                                          std::set<std::string> &set) const;
  void explore_node_for_vars_left_join_node(const proto_msg::LeftJoinNode &node,
                                            std::set<std::string> &set) const;
  void explore_node_for_vars_bgp_node(const proto_msg::BGPNode &node,
                                      std::set<std::string> &set) const;
  void explore_term_for_vars(const proto_msg::RDFTerm &term,
                             std::set<std::string> &set) const;
  void explore_expr_for_vars(const proto_msg::ExprNode &node,
                             std::set<std::string> &set) const;
  void explore_fun_expr_for_vars(const proto_msg::FunctionNode &fun_node,
                                 std::set<std::string> &set) const;
  void explore_union_for_vars(const proto_msg::UnionNode &node,
                              std::set<std::string> &set) const;
  void explore_filter_node_for_vars(const proto_msg::FilterNode &node,
                                    std::set<std::string> &set) const;
  void explore_extend_node_for_vars(const proto_msg::ExtendNode &node,
                                    std::set<std::string> &set) const;
  void explore_sequence_node_for_vars(const proto_msg::SequenceNode &node,
                                      std::set<std::string> &set) const;
  void bind_row_vars_next_eval_data(NaiveDynamicStringDictionary &dictionary,
                                    BoundVarsMap &bound_vars_map,
                                    const row_t &row);
  void bind_vars_to_sparql_tree(BoundVarsMap &map,
                                proto_msg::SparqlTree &sparql_tree);
  void bind_vars_to_node(BoundVarsMap &map, proto_msg::SparqlNode &node);
  void bind_vars_to_project_node(BoundVarsMap &map,
                                 proto_msg::ProjectNode &node);
  void replace_vars(BoundVarsMap &map, proto_msg::ProjectNode &node);
  void bind_vars_to_bgp_node(BoundVarsMap &map, proto_msg::BGPNode &bgp_node);
  void bind_vars_to_term_node(BoundVarsMap &map, proto_msg::RDFTerm &term);
  void bind_vars_to_expr_node(BoundVarsMap &map,
                              proto_msg::ExprNode &expr_node);
  void bind_vars_to_function_expr_node(BoundVarsMap &map,
                                       proto_msg::FunctionNode &node);
  void bind_vars_to_union_node(BoundVarsMap &map, proto_msg::UnionNode &node);
  void bind_vars_to_filter_node(BoundVarsMap &map, proto_msg::FilterNode &node);
  void bind_vars_to_extend_node(BoundVarsMap &map, proto_msg::ExtendNode &node);
  void bind_vars_to_sequence_node(BoundVarsMap &map,
                                  proto_msg::SequenceNode &node);
};

#endif // RDFCACHEK2_EXISTSEVAL_HPP
