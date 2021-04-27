//
// Created by cristobal on 4/20/21.
//

#include "DivideEval.hpp"
#include "DoubleResource.hpp"
#include "FloatResource.hpp"
#include "IntegerResource.hpp"

int DivideEval::eval_integer(const ExprEval::row_t &row) {
  auto dividend = children[0]->eval_integer(row);
  auto divisor = children[1]->eval_integer(row);
  if(children_with_error() || divisor == 0){
    this->with_error = true;
    return 0;
  }
  return dividend / divisor;
}
float DivideEval::eval_float(const ExprEval::row_t &row) {
  auto dividend = children[0]->eval_float(row);
  auto divisor = children[1]->eval_float(row);
  if(children_with_error() || divisor == 0){
    this->with_error = true;
    return 0;
  }
  return dividend / divisor;
}

double DivideEval::eval_double(const ExprEval::row_t &row) {
  auto dividend = children[0]->eval_double(row);
  auto divisor = children[1]->eval_double(row);
  if(children_with_error() || divisor == 0){
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
std::unique_ptr<TermResource>
DivideEval::eval_resource(const ExprEval::row_t &row) {
  auto dividend = children[0]->eval_resource(row);
  auto divisor = children[1]->eval_resource(row);

  if(children_with_error()){
    this->with_error = true;
    return TermResource::null();
  }

  if(dividend->is_double() || divisor->is_double()){
    if(divisor->get_double() == 0){
      this->with_error = true;
      return TermResource::null();
    }
    return std::make_unique<DoubleResource>(dividend->get_double() / divisor->get_double());
  }
  if(dividend->is_float() || divisor->is_float()){
    if(divisor->get_float() == 0){
      this->with_error = true;
      return TermResource::null();
    }
    return std::make_unique<FloatResource>(dividend->get_float() / divisor->get_float());
  }

  if(divisor->get_integer() == 0){
    this->with_error = true;
    return TermResource::null();
  }

  return std::make_unique<IntegerResource>(dividend->get_float() / divisor->get_float());


}
