//
// Created by cristobal on 4/20/21.
//

#include "SubtractEval.hpp"

#include "DoubleResource.hpp"
#include "FloatResource.hpp"
#include "IntegerResource.hpp"

int SubtractEval::eval_integer(const ExprEval::row_t &row) {
  auto left_value = children[0]->eval_integer(row);
  auto right_value = children[1]->eval_integer(row);
  if (children_with_error()) {
    this->with_error = true;
    return 0;
  }
  return left_value - right_value;
}

float SubtractEval::eval_float(const ExprEval::row_t &row) {
  auto left_value = children[0]->eval_float(row);
  auto right_value = children[1]->eval_float(row);
  if (children_with_error()) {
    this->with_error = true;
    return 0;
  }
  return left_value - right_value;
}
double SubtractEval::eval_double(const ExprEval::row_t &row) {
  auto left_value = children[0]->eval_double(row);
  auto right_value = children[1]->eval_double(row);
  if (children_with_error()) {
    this->with_error = true;
    return 0;
  }
  return left_value - right_value;
}

void SubtractEval::init() {
  ExprEval::init();
  add_children();
}
void SubtractEval::validate() {
  ExprEval::validate();
  assert_fsize(2);
}
std::unique_ptr<TermResource>
SubtractEval::eval_resource(const ExprEval::row_t &row) {
  auto left_resource = children[0]->eval_resource(row);
  auto right_resource = children[1]->eval_resource(row);

  if (children_with_error() || !left_resource->is_numeric() ||
      !right_resource->is_numeric()) {
    with_error = true;
    return TermResource::null();
  }

  if (left_resource->is_double() || right_resource->is_double()) {
    return std::make_unique<DoubleResource>(left_resource->get_double() -
                                            right_resource->get_double());
  }
  if (left_resource->is_float() || right_resource->is_float()) {
    return std::make_unique<FloatResource>(left_resource->get_float() -
                                           right_resource->get_float());
  }

  return std::make_unique<IntegerResource>(left_resource->get_integer() -
                                           right_resource->get_integer());
}
