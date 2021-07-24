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
  std::shared_ptr<VarBindingQProc>
  bind_row_vars_next_eval_data(NaiveDynamicStringDictionary &dictionary,
                               const row_t &row);
};

#endif // RDFCACHEK2_EXISTSEVAL_HPP
