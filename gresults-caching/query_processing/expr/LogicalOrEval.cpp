//
// Created by cristobal on 4/20/21.
//

#include "LogicalOrEval.hpp"
#include "query_processing/resources/BooleanResource.hpp"
std::shared_ptr<TermResource>
LogicalOrEval::eval_resource(const ExprEval::row_t &row) {
  return generate_from_eval_boolean(row);
}
bool LogicalOrEval::eval_boolean(const ExprEval::row_t &row) {
  auto first_resource = children[0]->produce_boolean(row);
  if (children[0]->has_error()) {
    this->with_error = true;
    return false;
  }
  if (first_resource)
    return true;
  auto second_resource = children[1]->produce_boolean(row);
  if (children[1]->has_error()) {
    this->with_error = true;
    return false;
  }
  return second_resource;
}

void LogicalOrEval::validate() {
  ExprEval::validate();
  assert_fun_size(2);
}
void LogicalOrEval::init() {
  ExprEval::init();
  add_children();
}
