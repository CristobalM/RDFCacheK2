//
// Created by cristobal on 4/20/21.
//

#include "NotEqualsEval.hpp"
#include "BooleanResource.hpp"

void NotEqualsEval::validate() {
  ExprEval::validate();
  assert_fsize(2);
}
void NotEqualsEval::init() {
  ExprEval::init();
  add_children();
}
bool NotEqualsEval::eval_boolean(const ExprEval::row_t &row) {
  auto left_resource = children[0]->eval_resource(row);
  auto right_resource = children[1]->eval_resource(row);
  if (children_with_error()) {
    this->with_error = true;
    return false;
  }
  return left_resource != right_resource;
}

std::unique_ptr<TermResource>
NotEqualsEval::eval_resource(const ExprEval::row_t &row) {
  return generate_from_eval_boolean(row);
}
