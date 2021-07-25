//
// Created by cristobal on 4/20/21.
//

#include "LogicalNotEval.hpp"
#include "query_processing/resources/BooleanResource.hpp"

std::shared_ptr<TermResource>
LogicalNotEval::eval_resource(const ExprEval::row_t &row) {
  return generate_from_eval_boolean(row);
}
bool LogicalNotEval::eval_boolean(const ExprEval::row_t &row) {
  auto result = children[0]->eval_boolean(row);
  if (children_with_error()) {
    this->with_error = true;
    return false;
  }
  return !result;
}
void LogicalNotEval::validate() {
  ExprEval::validate();
  assert_fun_size(1);
}
void LogicalNotEval::init() {
  ExprEval::init();
  add_children();
}
