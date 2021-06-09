//
// Created by cristobal on 4/20/21.
//

#include "UnaryPlusEval.hpp"
#include "query_processing/resources/DoubleResource.hpp"
#include "query_processing/resources/FloatResource.hpp"
#include "query_processing/resources/IntegerResource.hpp"
int UnaryPlusEval::eval_integer(const ExprEval::row_t &row) {
  int value = children[0]->eval_integer(row);
  if (children_with_error())
    return integer_with_error();
  return value;
}
float UnaryPlusEval::eval_float(const ExprEval::row_t &row) {
  float value = children[0]->eval_float(row);
  if (children_with_error())
    return float_with_error();
  return value;
}
double UnaryPlusEval::eval_double(const ExprEval::row_t &row) {
  double value = children[0]->eval_double(row);
  if (children_with_error())
    return double_with_error();
  return value;
}

std::shared_ptr<TermResource>
UnaryPlusEval::eval_resource(const ExprEval::row_t &row) {
  auto child_resource = children[0]->eval_resource(row);
  if (children_with_error())
    return resource_with_error();
  return child_resource;
}

void UnaryPlusEval::init() {
  ExprEval::init();
  assert_fsize(1);
}
void UnaryPlusEval::validate() {
  ExprEval::validate();
  add_children();
}
