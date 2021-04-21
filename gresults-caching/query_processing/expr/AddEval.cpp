//
// Created by cristobal on 4/20/21.
//

#include "AddEval.hpp"
int AddEval::eval_integer(const ExprEval::row_t &row) {
  return children[0]->eval_integer(row) + children[1]->eval_integer(row);
}
float AddEval::eval_float(const ExprEval::row_t &row) {
  return children[0]->eval_float(row) + children[1]->eval_float(row);
}
double AddEval::eval_double(const ExprEval::row_t &row) {
  return children[0]->eval_double(row) + children[1]->eval_double(row);
}

void AddEval::init() {
  ExprEval::init();
  add_child(expr_node.function_node().exprs(0));
  add_child(expr_node.function_node().exprs(1));
}
void AddEval::validate() {
  ExprEval::validate();
  assert_fsize(2);
}
