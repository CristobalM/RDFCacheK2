//
// Created by cristobal on 6/4/21.
//

#include "DateTimeResource.hpp"
#include "StringLiteralResource.hpp"
#include <query_processing/ParsingUtils.hpp>
bool DateTimeResource::is_datetime() const { return true; }
const std::string &DateTimeResource::get_literal_string() const {
  return TermResource::get_literal_string();
}
RDFResource DateTimeResource::get_resource_clone() const {
  return RDFResource(get_content_string_copy(), RDF_TYPE_LITERAL);
}
ExprDataType DateTimeResource::get_datatype() const {
  return ExprDataType::EDT_DATETIME;
}
DateInfo DateTimeResource::get_dateinfo() const { return date_info; }
DateTimeResource::DateTimeResource(DateInfo date_info) : date_info(date_info) {}
bool DateTimeResource::operator==(const TermResource &rhs) const {
  if (!rhs.is_concrete() && !rhs.is_datetime() && !rhs.is_string_literal())
    return false;

  if (rhs.is_datetime())
    return date_info == rhs.get_dateinfo();

  if (rhs.is_string_literal()) {
    auto literal_string = rhs.get_literal_string();
    const auto rhs_dateinfo = ParsingUtils::parse_iso8601(literal_string);
    return date_info == rhs_dateinfo;
  }

  // then it must be concrete

  return date_info ==
         ParsingUtils::parse_iso8601(rhs.get_content_string_copy());
}
int DateTimeResource::reverse_diff_compare(
    const DateTimeResource &date_time_resource) const {
  return date_info.rev_cmp_to(date_time_resource.get_dateinfo());
}

std::shared_ptr<TermResource>
DateTimeResource::cast_to(ExprDataType expr_data_type) {
  if (expr_data_type != EDT_STRING)
    return TermResource::null();

  return std::make_shared<StringLiteralResource>(
      ParsingUtils::date_info_to_iso8601_string(date_info), EDT_DATETIME);
}
int DateTimeResource::diff_compare(const TermResource &rhs) const {
  return rhs.reverse_diff_compare(*this);
}
bool DateTimeResource::can_cast_to_literal_string() const { return true; }
std::string DateTimeResource::get_content_string_copy() const {
  return ParsingUtils::date_info_to_iso8601_string(date_info);
}
