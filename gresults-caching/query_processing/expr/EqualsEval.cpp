//
// Created by cristobal on 4/20/21.
//

#include "EqualsEval.hpp"
#include "query_processing/resources/BooleanResource.hpp"

void EqualsEval::validate() {
  ExprEval::validate();
  assert_fsize(2);
}
void EqualsEval::init() {
  ExprEval::init();
  add_children();
}
bool EqualsEval::eval_boolean(const ExprEval::row_t &row) {
  auto left_resource = children[0]->produce_resource(row);
  auto right_resource = children[1]->produce_resource(row);
  if (children_with_error()) {
    this->with_error = true;
    return false;
  }
  return *left_resource == *right_resource;
}

std::shared_ptr<TermResource>
EqualsEval::eval_resource(const ExprEval::row_t &row) {
  auto result = eval_boolean(row);
  if (has_error()) {
    return TermResource::null();
  }
  return std::make_shared<BooleanResource>(result);
}
