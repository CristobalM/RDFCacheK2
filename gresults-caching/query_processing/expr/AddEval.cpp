//
// Created by cristobal on 4/20/21.
//

#include "AddEval.hpp"
#include "query_processing/resources/DoubleResource.hpp"
#include "query_processing/resources/FloatResource.hpp"
#include "query_processing/resources/IntegerResource.hpp"

int AddEval::eval_integer(const ExprEval::row_t &row) {
  auto left_value = children[0]->produce_integer(row);
  auto right_value = children[1]->produce_integer(row);
  if (children_with_error()) {
    this->with_error = true;
    return 0;
  }
  return left_value + right_value;
}

float AddEval::eval_float(const ExprEval::row_t &row) {
  auto left_value = children[0]->produce_float(row);
  auto right_value = children[1]->produce_float(row);
  if (children_with_error()) {
    this->with_error = true;
    return 0;
  }
  return left_value + right_value;
}
double AddEval::eval_double(const ExprEval::row_t &row) {
  auto left_value = children[0]->produce_double(row);
  auto right_value = children[1]->produce_double(row);
  if (children_with_error()) {
    this->with_error = true;
    return 0;
  }
  return left_value + right_value;
}

void AddEval::init() {
  ExprEval::init();
  add_children();
}
void AddEval::validate() {
  ExprEval::validate();
  assert_fun_size(2);
}
std::shared_ptr<TermResource>
AddEval::eval_resource(const ExprEval::row_t &row) {
  auto left_resource = children[0]->produce_resource(row);
  auto right_resource = children[1]->produce_resource(row);

  if (children_with_error() || !left_resource->is_numeric() ||
      !right_resource->is_numeric()) {
    with_error = true;
    return TermResource::null();
  }

  if (left_resource->is_double() || right_resource->is_double()) {
    return std::make_shared<DoubleResource>(left_resource->get_double() +
                                            right_resource->get_double());
  }
  if (left_resource->is_float() || right_resource->is_float()) {
    return std::make_shared<FloatResource>(left_resource->get_float() +
                                           right_resource->get_float());
  }

  return std::make_shared<IntegerResource>(left_resource->get_integer() +
                                           right_resource->get_integer());
}
