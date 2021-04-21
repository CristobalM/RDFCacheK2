//
// Created by cristobal on 4/20/21.
//

#include "BoundEval.hpp"

bool BoundEval::eval_boolean(const ExprEval::row_t &) { return result; }

void BoundEval::init() {
  ExprEval::init();
  const auto &child_node = expr_node.function_node().exprs(0);
  const auto &var = child_node.term_node().term_value();
  result = this->eval_data.var_pos_mapping.find(var) !=
           this->eval_data.var_pos_mapping.end();
}

void BoundEval::validate() {
  ExprEval::validate();
  assert_fsize(1);
  const auto &child_node = expr_node.function_node().exprs(0);
  assert_is_variable(child_node);
}
