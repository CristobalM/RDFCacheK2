//
// Created by cristobal on 4/20/21.
//

#include "LessThanEval.hpp"
#include "query_processing/resources/BooleanResource.hpp"
std::shared_ptr<TermResource>
LessThanEval::eval_resource(const ExprEval::row_t &row) {
  return generate_from_eval_boolean(row);
}
bool LessThanEval::eval_boolean(const ExprEval::row_t &row) {
  auto left_resource = children[0]->produce_resource(row);
  auto right_resource = children[1]->produce_resource(row);
  if (children[0]->has_error() || children[1]->has_error() ||
      !left_resource->is_numeric() || !right_resource->is_numeric()) {
    this->with_error = true;
    return false;
  }

  return left_resource->get_double() < right_resource->get_double();
}
void LessThanEval::validate() {
  ExprEval::validate();
  assert_fun_size(2);
}
void LessThanEval::init() {
  ExprEval::init();
  add_children();
}
