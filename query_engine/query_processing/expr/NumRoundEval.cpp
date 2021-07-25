//
// Created by cristobal on 4/20/21.
//

#include "NumRoundEval.hpp"
#include "query_processing/resources/DoubleResource.hpp"
#include "query_processing/resources/FloatResource.hpp"
#include "query_processing/resources/IntegerResource.hpp"
#include <cmath>
std::shared_ptr<TermResource>
NumRoundEval::eval_resource(const ExprEval::row_t &row) {
  auto resource = children[0]->eval_resource(row);
  if (children_with_error()) {
    this->with_error = true;
    return TermResource::null();
  }

  if (resource->is_double()) {
    return std::make_shared<DoubleResource>(std::round(resource->get_double()));
  }
  if (resource->is_float()) {
    return std::make_shared<FloatResource>(std::round(resource->get_float()));
  }
  if (resource->is_integer()) {
    return std::make_shared<IntegerResource>(
        std::round(resource->get_integer()));
  }

  this->with_error = true;
  return TermResource::null();
}
int NumRoundEval::eval_integer(const ExprEval::row_t &row) {
  auto inner_value = children[0]->eval_integer(row);
  if (children_with_error()) {
    this->with_error = true;
    return 0;
  }
  return std::round(inner_value);
}

float NumRoundEval::eval_float(const ExprEval::row_t &row) {
  auto inner_value = children[0]->eval_float(row);
  if (children_with_error()) {
    this->with_error = true;
    return 0;
  }
  return std::round(inner_value);
}
double NumRoundEval::eval_double(const ExprEval::row_t &row) {
  auto inner_value = children[0]->eval_double(row);
  if (children_with_error()) {
    this->with_error = true;
    return 0;
  }
  return std::round(inner_value);
}
void NumRoundEval::validate() {
  ExprEval::validate();
  assert_fun_size(1);
}
void NumRoundEval::init() {
  ExprEval::init();
  add_children();
}
