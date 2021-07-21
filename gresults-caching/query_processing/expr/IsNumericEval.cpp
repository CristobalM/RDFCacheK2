//
// Created by cristobal on 4/20/21.
//

#include "IsNumericEval.hpp"
bool IsNumericEval::eval_boolean(const ExprEval::row_t &row) {
  auto resource = children[0]->eval_datatype(row);
  if (children_with_error() || !resource->is_datatype()) {
    this->with_error = true;
    return false;
  }
  auto datatype = resource->get_datatype();
  return datatype == ExprDataType::EDT_DECIMAL ||
         datatype == ExprDataType::EDT_DOUBLE ||
         datatype == ExprDataType::EDT_FLOAT ||
         datatype == ExprDataType::EDT_INTEGER;
}

void IsNumericEval::validate() {
  ExprEval::validate();
  assert_fun_size(1);
}
void IsNumericEval::init() {
  ExprEval::init();
  add_children();
}
std::shared_ptr<TermResource>
IsNumericEval::eval_resource(const ExprEval::row_t &row) {
  return generate_from_eval_boolean(row);
}
