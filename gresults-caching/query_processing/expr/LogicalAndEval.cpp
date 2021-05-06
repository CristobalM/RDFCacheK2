//
// Created by cristobal on 4/20/21.
//

#include "LogicalAndEval.hpp"
#include "BooleanResource.hpp"
std::unique_ptr<TermResource>
LogicalAndEval::eval_resource(const ExprEval::row_t &row) {
  return generate_from_eval_boolean(row);
}
bool LogicalAndEval::eval_boolean(const ExprEval::row_t &row) {
  auto first_resource = children[0]->eval_boolean(row);
  auto second_resource = children[1]->eval_boolean(row);
  if (children_with_error()) {
    this->with_error = true;
    return false;
  }
  return first_resource && second_resource;
}
void LogicalAndEval::validate() {
  ExprEval::validate();
  assert_fsize(2);
}
void LogicalAndEval::init() {
  ExprEval::init();
  add_children();
}
