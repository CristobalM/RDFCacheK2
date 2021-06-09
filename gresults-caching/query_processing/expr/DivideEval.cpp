//
// Created by cristobal on 4/20/21.
//

#include "DivideEval.hpp"
#include "query_processing/resources/DoubleResource.hpp"
#include "query_processing/resources/FloatResource.hpp"
#include "query_processing/resources/IntegerResource.hpp"

int DivideEval::eval_integer(const ExprEval::row_t &row) {
  auto dividend = children[0]->produce_integer(row);
  auto divisor = children[1]->produce_integer(row);
  if (children_with_error() || divisor == 0) {
    this->with_error = true;
    return 0;
  }
  return dividend / divisor;
}
float DivideEval::eval_float(const ExprEval::row_t &row) {
  auto dividend = children[0]->produce_float(row);
  auto divisor = children[1]->produce_float(row);
  if (children_with_error() || divisor == 0) {
    this->with_error = true;
    return 0;
  }
  return dividend / divisor;
}

double DivideEval::eval_double(const ExprEval::row_t &row) {
  auto dividend = children[0]->produce_double(row);
  auto divisor = children[1]->produce_double(row);
  if (children_with_error() || divisor == 0) {
    this->with_error = true;
    return 0;
  }
  return dividend / divisor;
}

void DivideEval::init() {
  ExprEval::init();
  add_children();
}
void DivideEval::validate() {
  ExprEval::validate();
  assert_fsize(2);
}
std::shared_ptr<TermResource>
DivideEval::eval_resource(const ExprEval::row_t &row) {
  auto dividend = children[0]->produce_resource(row);
  auto divisor = children[1]->produce_resource(row);

  if (children_with_error()) {
    this->with_error = true;
    return TermResource::null();
  }

  if (dividend->is_double() || divisor->is_double()) {
    if (divisor->get_double() == 0) {
      this->with_error = true;
      return TermResource::null();
    }
    return std::make_shared<DoubleResource>(dividend->get_double() /
                                            divisor->get_double());
  }
  if (dividend->is_float() || divisor->is_float()) {
    if (divisor->get_float() == 0) {
      this->with_error = true;
      return TermResource::null();
    }
    return std::make_shared<FloatResource>(dividend->get_float() /
                                           divisor->get_float());
  }

  if (divisor->get_integer() == 0) {
    this->with_error = true;
    return TermResource::null();
  }

  return std::make_shared<IntegerResource>(dividend->get_float() /
                                           divisor->get_float());
}
