//
// Created by cristobal on 4/20/21.
//

#include "GreaterThanOrEqualEval.hpp"
#include "BooleanResource.hpp"
std::unique_ptr<TermResource>
GreaterThanOrEqualEval::eval_resource(const ExprEval::row_t &row) {
  auto value = eval_boolean(row);
  if (has_error()) {
    return TermResource::null();
  }
  return std::make_unique<BooleanResource>(value);
}
bool GreaterThanOrEqualEval::eval_boolean(const ExprEval::row_t &row) {
  auto left_resource = children[0]->eval_resource(row);
  auto right_resource = children[1]->eval_resource(row);
  if (children[0]->has_error() || children[1]->has_error() ||
      !left_resource->is_numeric() || !right_resource->is_numeric()) {
    this->with_error = true;
    return false;
  }

  return left_resource->get_double() >= right_resource->get_double();
}
void GreaterThanOrEqualEval::validate() {
  ExprEval::validate();
  assert_fsize(2);
}
void GreaterThanOrEqualEval::init() {
  ExprEval::init();
  add_children();
}
