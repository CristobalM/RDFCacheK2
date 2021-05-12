//
// Created by cristobal on 4/20/21.
//

#include "SameTermEval.hpp"
#include "BooleanResource.hpp"
std::shared_ptr<TermResource>
SameTermEval::eval_resource(const ExprEval::row_t &row) {
  return generate_from_eval_boolean(row);
}
bool SameTermEval::eval_boolean(const ExprEval::row_t &row) {
  auto first_resource = children[0]->produce_resource(row);
  auto second_resource = children[1]->produce_resource(row);
  if (children_with_error()) {
    this->with_error = true;
    return false;
  }

  return *first_resource == *second_resource;
}
void SameTermEval::validate() {
  ExprEval::validate();
  assert_fsize(2);
}
void SameTermEval::init() {
  ExprEval::init();
  add_children();
}
