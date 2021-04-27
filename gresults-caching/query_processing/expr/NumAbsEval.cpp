//
// Created by cristobal on 4/20/21.
//

#include "NumAbsEval.hpp"
#include "DoubleResource.hpp"
#include "FloatResource.hpp"
#include "IntegerResource.hpp"
#include <cmath>

std::unique_ptr<TermResource>
NumAbsEval::eval_resource(const ExprEval::row_t &row) {
  auto resource = children[0]->eval_resource(row);
  if (children_with_error()) {
    this->with_error = true;
    return TermResource::null();
  }

  if (resource->is_double()) {
    return std::make_unique<DoubleResource>(std::abs(resource->get_double()));
  }
  if (resource->is_float()) {
    return std::make_unique<FloatResource>(std::abs(resource->get_float()));
  }
  if (resource->is_integer()) {
    return std::make_unique<IntegerResource>(std::abs(resource->get_integer()));
  }

  this->with_error = true;
  return TermResource::null();
}
int NumAbsEval::eval_integer(const ExprEval::row_t &row) {
  auto inner_value = children[0]->eval_integer(row);
  if (children_with_error()) {
    this->with_error = true;
    return 0;
  }
  return std::abs(inner_value);
}

float NumAbsEval::eval_float(const ExprEval::row_t &row) {
  auto inner_value = children[0]->eval_float(row);
  if (children_with_error()) {
    this->with_error = true;
    return 0;
  }
  return std::abs(inner_value);
}
double NumAbsEval::eval_double(const ExprEval::row_t &row) {
  auto inner_value = children[0]->eval_double(row);
  if (children_with_error()) {
    this->with_error = true;
    return 0;
  }
  return std::abs(inner_value);
}
void NumAbsEval::validate() {
  ExprEval::validate();
  assert_fsize(1);
}
void NumAbsEval::init() {
  ExprEval::init();
  add_children();
}
