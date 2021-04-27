//
// Created by cristobal on 4/20/21.
//

#include "DateTimeTZEval.hpp"
#include "DayTimeDurationResource.hpp"
#include <unicode/smpdtfmt.h>

void DateTimeTZEval::validate() {
  ExprEval::validate();
  assert_fsize(1);
}

void DateTimeTZEval::init() {
  ExprEval::init();
  add_children();
}
std::unique_ptr<TermResource>
DateTimeTZEval::eval_resource(const ExprEval::row_t &row) {
  auto date_info = children[0]->eval_date_time(row);
  if (children_with_error()) {
    with_error = true;
    return TermResource::null();
  }

  return std::make_unique<DayTimeDurationResource>(date_info.offset_sign, 0,
                                                   date_info.offset_hour,
                                                   date_info.offset_minute, 0);
}
