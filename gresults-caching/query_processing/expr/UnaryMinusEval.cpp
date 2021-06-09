//
// Created by cristobal on 4/20/21.
//

#include "UnaryMinusEval.hpp"
#include "query_processing/resources/DoubleResource.hpp"
#include "query_processing/resources/FloatResource.hpp"
#include "query_processing/resources/IntegerResource.hpp"
int UnaryMinusEval::eval_integer(const ExprEval::row_t &row) {
  int value = children[0]->eval_integer(row);
  if (children_with_error())
    return integer_with_error();
  return -value;
}
float UnaryMinusEval::eval_float(const ExprEval::row_t &row) {
  float value = children[0]->eval_float(row);
  if (children_with_error())
    return float_with_error();
  return -value;
}
double UnaryMinusEval::eval_double(const ExprEval::row_t &row) {
  double value = children[0]->eval_double(row);
  if (children_with_error())
    return double_with_error();
  return -value;
}

std::shared_ptr<TermResource>
UnaryMinusEval::eval_resource(const ExprEval::row_t &row) {
  auto child_resource = children[0]->eval_resource(row);
  if (children_with_error())
    return resource_with_error();
  if (child_resource->is_integer())
    return std::make_shared<IntegerResource>(-child_resource->get_integer());
  if (child_resource->is_float())
    return std::make_shared<FloatResource>(-child_resource->get_float());
  if (child_resource->is_double())
    return std::make_shared<DoubleResource>(-child_resource->get_double());
  return resource_with_error();
}

void UnaryMinusEval::init() {
  ExprEval::init();
  assert_fsize(1);
}
void UnaryMinusEval::validate() {
  ExprEval::validate();
  add_children();
}
