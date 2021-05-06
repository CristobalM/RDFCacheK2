//
// Created by cristobal on 4/20/21.
//

#include "DateTimeMinutesEval.hpp"
#include <unicode/smpdtfmt.h>

int DateTimeMinutesEval::eval_integer(const ExprEval::row_t &row) {
  auto date_info = children[0]->eval_date_time(row);
  if (children_with_error()) {
    with_error = true;
    return 0;
  }
  return date_info.minute;
}

void DateTimeMinutesEval::validate() {
  ExprEval::validate();
  assert_fsize(1);
}
void DateTimeMinutesEval::init() {
  ExprEval::init();
  add_children();
}
std::unique_ptr<TermResource>
DateTimeMinutesEval::eval_resource(const ExprEval::row_t &row) {
  return generate_from_eval_integer(row);
}
