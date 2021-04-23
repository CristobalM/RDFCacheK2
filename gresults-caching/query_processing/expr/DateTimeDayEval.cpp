//
// Created by cristobal on 4/20/21.
//

#include "DateTimeDayEval.hpp"

#include <unicode/gregocal.h>


int DateTimeDayEval::eval_integer(const ExprEval::row_t &row) {
  auto dtepoch = children[0]->eval_date_time(row);
  UErrorCode err = U_ZERO_ERROR;
  icu::GregorianCalendar gregorian_calendar(err);
  gregorian_calendar.setTime(dtepoch, err);
  return gregorian_calendar.get(icu::Calendar::EDateFields::DAY_OF_WEEK_IN_MONTH, err);
}

void DateTimeDayEval::validate() {
  ExprEval::validate();
  assert_fsize(1);
}
void DateTimeDayEval::init() {
  ExprEval::init();
  add_children();
}
