//
// Created by cristobal on 4/20/21.
//

#include "ConditionalEval.hpp"
std::unique_ptr<TermResource>
ConditionalEval::eval_resource(const ExprEval::row_t &row) {
  if (children[0]->eval_boolean(row))
    return children[1]->eval_resource(row);
  else
    return children[2]->eval_resource(row);
}

void ConditionalEval::validate() {
  ExprEval::validate();
  assert_fsize(3);
  assert_is_function(expr_node);
}
void ConditionalEval::init() {
  ExprEval::init();
  const auto &function_node = expr_node.function_node();
  add_children();
}
