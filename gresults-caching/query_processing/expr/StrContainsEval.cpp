//
// Created by cristobal on 4/20/21.
//

#include "StrContainsEval.hpp"
std::unique_ptr<TermResource>
StrContainsEval::eval_resource(const ExprEval::row_t &row) {
  return generate_from_eval_boolean(row);
}
bool StrContainsEval::eval_boolean(const ExprEval::row_t &row) {
  auto input_resource = children[0]->eval_resource(row);
  auto pattern_resource = children[1]->eval_resource(row);

  if (children_with_error()) {
    this->with_error = true;
    return false;
  }

  return input_resource->contains(*pattern_resource);
}
void StrContainsEval::validate() {
  ExprEval::validate();
  assert_fsize(2);
}
void StrContainsEval::init() {
  ExprEval::init();
  add_children();
}
