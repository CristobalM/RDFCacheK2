//
// Created by cristobal on 4/20/21.
//

#include "CoalesceEval.hpp"
#include "ConcreteRDFResource.hpp"
std::unique_ptr<TermResource>
CoalesceEval::eval_resource(const ExprEval::row_t &row) {
  for (const auto &child : this->children) {
    auto resource = child->eval_resource(row);
    if (!child->has_error())
      return resource;
  }
  this->with_error = true;
  return TermResource::null();
}

void CoalesceEval::validate() { ExprEval::validate(); }
void CoalesceEval::init() {
  ExprEval::init();
  const auto &fun_node = expr_node.function_node();
  add_children();
}
