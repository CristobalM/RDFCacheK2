//
// Created by cristobal on 4/20/21.
//

#include "DateTimeTZEval.hpp"
#include <unicode/smpdtfmt.h>

int DateTimeTZEval::eval_integer(const ExprEval::row_t &row) {
  auto dtepoch = children[0]->eval_date_time(row);
  if (children_with_error()) {
    with_error = true;
    return 0;
  }
  return persistent_data.extract_date_portion(dtepoch,
                                              icu::Calendar::EDateFields::);
}

void DateTimeTZEval::validate() {
  ExprEval::validate();
  assert_fsize(1);
}

void DateTimeTZEval::init() {
  ExprEval::init();
  add_children();
}
