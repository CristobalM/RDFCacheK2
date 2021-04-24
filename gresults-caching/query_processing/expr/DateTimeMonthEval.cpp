//
// Created by cristobal on 4/20/21.
//

#include "DateTimeMonthEval.hpp"
#include <unicode/smpdtfmt.h>

int DateTimeMonthEval::eval_integer(const ExprEval::row_t &row) {
  auto dtepoch = children[0]->eval_date_time(row);
  if (children_with_error()) {
    with_error = true;
    return 0;
  }
  return persistent_data.extract_date_portion(
      dtepoch, icu::Calendar::EDateFields::MONTH);
}

void DateTimeMonthEval::validate() {
  ExprEval::validate();
  assert_fsize(1);
}

void DateTimeMonthEval::init() {
  ExprEval::init();
  add_children();
}
