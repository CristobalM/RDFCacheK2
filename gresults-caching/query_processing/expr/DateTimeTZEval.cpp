//
// Created by cristobal on 4/20/21.
//

#include "DateTimeTZEval.hpp"
#include "DayTimeDurationResource.hpp"
#include "StringLiteralResource.hpp"
#include <iomanip>
#include <sstream>
#include <unicode/smpdtfmt.h>

void DateTimeTZEval::validate() {
  ExprEval::validate();
  assert_fsize(1);
}

void DateTimeTZEval::init() {
  ExprEval::init();
  add_children();
}
std::shared_ptr<TermResource>
DateTimeTZEval::eval_resource(const ExprEval::row_t &row) {
  auto date_info = children[0]->eval_date_time(row);
  if (children_with_error()) {
    with_error = true;
    return TermResource::null();
  }

  if (date_info.offset_hour == 0 && date_info.offset_minute == 0)
    return std::make_shared<StringLiteralResource>("Z", EDT_UNKNOWN);

  return std::make_shared<StringLiteralResource>(format_offset(date_info),
                                                 EDT_UNKNOWN);
}
std::string DateTimeTZEval::format_offset(const DateInfo &info) {
  std::stringstream ss;
  ss << ((info.offset_sign >= 0) ? '+' : '-') << std::setw(2)
     << std::setfill('0') << info.offset_hour << ":" << std::setw(2)
     << std::setfill('0') << info.offset_minute;
  return ss.str();
}
