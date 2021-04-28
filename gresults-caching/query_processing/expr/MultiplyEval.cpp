//
// Created by cristobal on 4/20/21.
//

#include "MultiplyEval.hpp"
#include "DoubleResource.hpp"
#include "FloatResource.hpp"
#include "IntegerResource.hpp"

int MultiplyEval::eval_integer(const ExprEval::row_t &row) {
  auto first = children[0]->eval_integer(row);
  auto second = children[1]->eval_integer(row);
  if (children_with_error()) {
    this->with_error = true;
    return 0;
  }
  return first * second;
}
float MultiplyEval::eval_float(const ExprEval::row_t &row) {
  auto first = children[0]->eval_float(row);
  auto second = children[1]->eval_float(row);
  if (children_with_error()) {
    this->with_error = true;
    return 0;
  }
  return first * second;
}

double MultiplyEval::eval_double(const ExprEval::row_t &row) {
  auto first = children[0]->eval_double(row);
  auto second = children[1]->eval_double(row);
  if (children_with_error()) {
    this->with_error = true;
    return 0;
  }
  return first * second;
}

void MultiplyEval::init() {
  ExprEval::init();
  add_children();
}
void MultiplyEval::validate() {
  ExprEval::validate();
  assert_fsize(2);
}
std::unique_ptr<TermResource>
MultiplyEval::eval_resource(const ExprEval::row_t &row) {
  auto first = children[0]->eval_resource(row);
  auto second = children[1]->eval_resource(row);

  if (children_with_error()) {
    this->with_error = true;
    return TermResource::null();
  }

  if (first->is_double() || second->is_double()) {
    return std::make_unique<DoubleResource>(first->get_double() *
                                            second->get_double());
  }
  if (first->is_float() || second->is_float()) {
    return std::make_unique<FloatResource>(first->get_float() *
                                           second->get_float());
  }

  return std::make_unique<IntegerResource>(first->get_float() *
                                           second->get_float());
}
