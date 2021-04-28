//
// Created by cristobal on 4/20/21.
//

#include "LogicalNotEval.hpp"
#include "BooleanResource.hpp"

std::unique_ptr<TermResource>
LogicalNotEval::eval_resource(const ExprEval::row_t &row) {
  auto result = eval_boolean(row);
  if (has_error()) {
    return TermResource::null();
  }
  return std::make_unique<BooleanResource>(result);
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
  assert_fsize(1);
}
void LogicalNotEval::init() {
  ExprEval::init();
  add_children();
}
