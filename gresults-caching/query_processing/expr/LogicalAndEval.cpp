//
// Created by cristobal on 4/20/21.
//

#include "LogicalAndEval.hpp"
#include "query_processing/resources/BooleanResource.hpp"
std::shared_ptr<TermResource>
LogicalAndEval::eval_resource(const ExprEval::row_t &row) {
  return generate_from_eval_boolean(row);
}
bool LogicalAndEval::eval_boolean(const ExprEval::row_t &row) {
  auto first_resource = children[0]->produce_boolean(row);
  if (children_with_error()) {
    this->with_error = true;
    return false;
  }
  if (!first_resource)
    return false;
  auto second_resource = children[1]->produce_boolean(row);
  if (children_with_error()) {
    this->with_error = true;
    return false;
  }
  return second_resource;
}
void LogicalAndEval::validate() {
  ExprEval::validate();
  assert_fun_size(2);
}
void LogicalAndEval::init() {
  ExprEval::init();
  add_children();
}
