//
// Created by cristobal on 6/4/21.
//

#include "DateTimeResource.hpp"
#include "StringLiteralResource.hpp"
#include <query_processing/ExprProcessorPersistentData.hpp>
bool DateTimeResource::is_datetime() const {
  return TermResource::is_datetime();
}
const std::string &DateTimeResource::get_literal_string() const {
  return TermResource::get_literal_string();
}
RDFResource DateTimeResource::get_resource_clone() const {
  return TermResource::get_resource_clone();
}
ExprDataType DateTimeResource::get_datatype() const {
  return ExprDataType::EDT_DATETIME;
}
DateInfo DateTimeResource::get_dateinfo() const {
  return TermResource::get_dateinfo();
}
DateTimeResource::DateTimeResource(DateInfo date_info) : date_info(date_info) {}
bool DateTimeResource::operator==(const TermResource &rhs) const {
  if (!rhs.is_concrete() && !rhs.is_datetime() && !rhs.is_string_literal())
    return false;

  if (rhs.is_datetime())
    return date_info == rhs.get_dateinfo();

  if (rhs.is_string_literal()) {
    auto literal_string = rhs.get_literal_string();
    const auto rhs_dateinfo =
        ExprProcessorPersistentData::get().parse_iso8601(literal_string);
    return date_info == rhs_dateinfo;
  }

  // then it must be concrete

  return date_info == ExprProcessorPersistentData::get().parse_iso8601(
                          rhs.get_content_string_copy());
}
int DateTimeResource::reverse_diff_compare(
    const DateTimeResource &date_time_resource) const {
  auto other_date_info = ExprProcessorPersistentData::zero_offset_date_info(
      date_time_resource.get_dateinfo());
  auto this_date_info =
      ExprProcessorPersistentData::zero_offset_date_info(date_info);
  auto year_diff = other_date_info.year - this_date_info.year;
  if (year_diff != 0)
    return ExprProcessorPersistentData::normalize_diff_cmp(year_diff);
  auto month_diff = other_date_info.month - this_date_info.month;
  if (month_diff != 0)
    return ExprProcessorPersistentData::normalize_diff_cmp(month_diff);
  auto day_diff = other_date_info.day - this_date_info.day;
  if (day_diff != 0)
    return ExprProcessorPersistentData::normalize_diff_cmp(day_diff);
  auto hour_diff = other_date_info.hour - this_date_info.hour;
  if (hour_diff != 0)
    return ExprProcessorPersistentData::normalize_diff_cmp(hour_diff);
  auto minute_diff = other_date_info.minute - this_date_info.minute;
  if (minute_diff != 0)
    return ExprProcessorPersistentData::normalize_diff_cmp(minute_diff);
  auto second_diff = other_date_info.second - this_date_info.second;
  if (second_diff != 0)
    return ExprProcessorPersistentData::normalize_diff_cmp(second_diff);
  auto seconds_frac_diff =
      other_date_info.second_fraction - this_date_info.second_fraction;
  if (seconds_frac_diff != 0)
    return ExprProcessorPersistentData::normalize_diff_cmp(seconds_frac_diff);
  return 0;
}
std::shared_ptr<TermResource>
DateTimeResource::cast_to(ExprDataType expr_data_type) {
  if (expr_data_type != EDT_STRING)
    return TermResource::null();

  return std::make_shared<StringLiteralResource>(
      ExprProcessorPersistentData::date_info_to_iso8601_string(date_info),
      EDT_DATETIME);
}
