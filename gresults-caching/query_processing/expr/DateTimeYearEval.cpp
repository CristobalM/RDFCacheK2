//
// Created by cristobal on 4/20/21.
//

#include "DateTimeYearEval.hpp"
int DateTimeYearEval::eval_integer(const ExprEval::row_t &row) {
  auto date_info = children[0]->eval_date_time(row);
  if (children_with_error()) {
    with_error = true;
    return 0;
  }
  return date_info.year;
}

void DateTimeYearEval::validate() {
  ExprEval::validate();
  assert_fsize(1);
}

void DateTimeYearEval::init() {
  ExprEval::init();
  add_children();
}
