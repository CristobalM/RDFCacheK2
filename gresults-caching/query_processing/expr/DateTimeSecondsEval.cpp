//
// Created by cristobal on 5/5/21.
//

#include "DateTimeSecondsEval.hpp"
int DateTimeSecondsEval::eval_integer(const ExprEval::row_t &row) {
  auto date_info = children[0]->eval_date_time(row);
  if (children_with_error()) {
    with_error = true;
    return 0;
  }
  return date_info.second;
}
void DateTimeSecondsEval::validate() {
  ExprEval::validate();
  assert_fsize(1);
}
void DateTimeSecondsEval::init() {
  ExprEval::init();
  add_children();
}
std::unique_ptr<TermResource>
DateTimeSecondsEval::eval_resource(const ExprEval::row_t &row) {
  return generate_from_eval_integer(row);
}
