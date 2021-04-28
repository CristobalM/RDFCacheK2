//
// Created by cristobal on 4/20/21.
//

#include "NumRoundEval.hpp"
#include "DoubleResource.hpp"
#include "FloatResource.hpp"
#include "IntegerResource.hpp"
#include <cmath>
std::unique_ptr<TermResource>
NumRoundEval::eval_resource(const ExprEval::row_t &row) {
  auto resource = children[0]->eval_resource(row);
  if (children_with_error()) {
    this->with_error = true;
    return TermResource::null();
  }

  if (resource->is_double()) {
    return std::make_unique<DoubleResource>(std::round(resource->get_double()));
  }
  if (resource->is_float()) {
    return std::make_unique<FloatResource>(std::round(resource->get_float()));
  }
  if (resource->is_integer()) {
    return std::make_unique<IntegerResource>(
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
  assert_fsize(1);
}
void NumRoundEval::init() {
  ExprEval::init();
  add_children();
}
