//
// Created by cristobal on 4/20/21.
//

#include "CoalesceEval.hpp"
RDFResource CoalesceEval::eval_resource(const ExprEval::row_t &row) {
  for (const auto &child : this->children) {
    auto resource = child->eval_resource(row);
    if (!child->has_error())
      return resource;
  }
  this->with_error = true;
  return RDFResource::null_resource();
}

void CoalesceEval::validate() { ExprEval::validate(); }
void CoalesceEval::init() {
  ExprEval::init();
  const auto &fun_node = expr_node.function_node();
  for (int i = 0; i < fun_node.exprs_size(); i++) {
    const auto &expr = fun_node.exprs(i);
    add_child(expr);
  }
}
