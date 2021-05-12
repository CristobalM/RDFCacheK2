//
// Created by cristobal on 4/20/21.
//

#include "DateTimeDayEval.hpp"

#include <unicode/gregocal.h>

int DateTimeDayEval::eval_integer(const ExprEval::row_t &row) {
  auto date_info = children[0]->eval_date_time(row);
  if (children_with_error()) {
    with_error = true;
    return 0;
  }
  return date_info.day;
}

void DateTimeDayEval::validate() {
  ExprEval::validate();
  assert_fsize(1);
}
void DateTimeDayEval::init() {
  ExprEval::init();
  add_children();
}

std::shared_ptr<TermResource>
DateTimeDayEval::eval_resource(const ExprEval::row_t &row) {
  return generate_from_eval_integer(row);
}