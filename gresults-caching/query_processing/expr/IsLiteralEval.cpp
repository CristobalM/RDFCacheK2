//
// Created by cristobal on 4/20/21.
//

#include "IsLiteralEval.hpp"
#include "BooleanResource.hpp"

bool IsLiteralEval::eval_boolean(const ExprEval::row_t &row) {
  auto resource = children[0]->eval_resource(row);
  if (children_with_error()) {
    this->with_error = true;
    return false;
  }
  return resource->is_literal();
}

void IsLiteralEval::init() {
  ExprEval::init();
  add_children();
}
void IsLiteralEval::validate() {
  ExprEval::validate();
  assert_fsize(1);
}
std::unique_ptr<TermResource>
IsLiteralEval::eval_resource(const ExprEval::row_t &row) {
  return generate_from_eval_boolean(row);
}
