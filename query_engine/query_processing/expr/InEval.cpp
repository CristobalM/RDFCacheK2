//
// Created by cristobal on 6/7/21.
//

#include "InEval.hpp"
void InEval::init() {
  ExprEval::init();
  add_children();
}
std::shared_ptr<TermResource>
InEval::eval_resource(const ExprEval::row_t &row) {
  return generate_from_eval_boolean(row);
}

bool InEval::eval_boolean(const ExprEval::row_t &row) {
  if (children.size() == 1)
    return false;

  auto first_resource = children[0]->eval_resource(row);

  for (size_t i = 1; i < children.size(); i++) {
    auto current_resource = children[i]->eval_resource(row);
    if (first_resource == current_resource)
      return true;
  }
  return false;
}

void InEval::validate() {
  ExprEval::validate();
  if (expr_node.function_node().exprs_size() == 0)
    throw std::runtime_error(
        "Invalid size " +
        std::to_string(expr_node.function_node().exprs_size()) +
        ", expected > 0");
}
