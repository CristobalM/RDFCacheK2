//
// Created by cristobal on 4/20/21.
//

#include "NumFloorEval.hpp"
#include "query_processing/resources/DoubleResource.hpp"
#include "query_processing/resources/FloatResource.hpp"
#include "query_processing/resources/IntegerResource.hpp"
#include <cmath>
std::shared_ptr<TermResource>
NumFloorEval::eval_resource(const ExprEval::row_t &row) {
  auto resource = children[0]->eval_resource(row);
  if (children_with_error()) {
    this->with_error = true;
    return TermResource::null();
  }

  if (resource->is_double()) {
    return std::make_shared<DoubleResource>(std::floor(resource->get_double()));
  }
  if (resource->is_float()) {
    return std::make_shared<FloatResource>(std::floor(resource->get_float()));
  }
  if (resource->is_integer()) {
    return std::make_shared<IntegerResource>(
        std::floor(resource->get_integer()));
  }

  this->with_error = true;
  return TermResource::null();
}
int NumFloorEval::eval_integer(const ExprEval::row_t &row) {
  auto inner_value = children[0]->eval_integer(row);
  if (children_with_error()) {
    this->with_error = true;
    return 0;
  }
  return std::floor(inner_value);
}

float NumFloorEval::eval_float(const ExprEval::row_t &row) {
  auto inner_value = children[0]->eval_float(row);
  if (children_with_error()) {
    this->with_error = true;
    return 0;
  }
  return std::floor(inner_value);
}
double NumFloorEval::eval_double(const ExprEval::row_t &row) {
  auto inner_value = children[0]->eval_double(row);
  if (children_with_error()) {
    this->with_error = true;
    return 0;
  }
  return std::floor(inner_value);
}
void NumFloorEval::validate() {
  ExprEval::validate();
  assert_fsize(1);
}
void NumFloorEval::init() {
  ExprEval::init();
  add_children();
}
