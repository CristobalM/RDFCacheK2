//
// Created by cristobal on 4/20/21.
//

#include "TermEval.hpp"
RDFResource TermEval::eval_resource(const ExprEval::row_t &row) const {
  switch (expr_node.term_node().term_type()) {
  case proto_msg::TermType::VARIABLE:
    return eval_variable_get_resource(row);
  case proto_msg::TermType::LITERAL:
  case proto_msg::TermType::BLANK_NODE:
  case proto_msg::TermType::IRI:
    return RDFResource(expr_node.term_node());
  default:
    throw std::runtime_error("Unexpected term type");
  }
}

RDFResource
TermEval::eval_variable_get_resource(const ExprEval::row_t &row) const {
  const auto &term = this->expr_node.term_node();
  if (term.term_type() != proto_msg::TermType::VARIABLE)
    throw std::runtime_error("Expected a variable");
  const std::string &var_name = term.term_value();
  if (this->eval_data.var_pos_mapping.find(var_name) ==
      this->eval_data.var_pos_mapping.end())
    throw std::runtime_error("Variable " + var_name + " not in table");
  auto pos = this->eval_data.var_pos_mapping.at(var_name);
  auto value_id = row[pos];
  auto resource = this->eval_data.cm.extract_resource(value_id);
  return resource;
}

void TermEval::validate() {
  ExprEval::validate();
  if (expr_node.expr_case() != proto_msg::ExprNode::kTermNode)
    throw std::runtime_error("Expected a term");
}
