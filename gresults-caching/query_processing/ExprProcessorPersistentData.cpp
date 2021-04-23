

//
// Created by Cristobal Miranda, 2020
//
#include "ExprProcessorPersistentData.hpp"

namespace {
std::string dtype = "(integer|decimal|float|double|string|boolean|dateTime)";
std::string datatype_regex = "\\^\\^(?:<http://www\\.w3\\.org/2001/XMLSchema#" +
                             dtype + ">|xsd:" + dtype + ")";

const std::string iri_letter = "[^\\x00-\\x20<>\"\\{\\}|\\^`\\\\]";
const std::string hex_letter = "[0-9a-fA-F]";
const std::string uchar_word = "\\\\u(?:" + hex_letter + "{8}|" + hex_letter + "{4})";
const std::string iri_valid = "(?:" + iri_letter + "|" + uchar_word + ")*";
const std::string iri_ref = "(?:<" + iri_valid +  ">)";
const std::string echar = "\\\\[tbnrf\"'\\\\]";
const std::string string_literal_q = "\"((?:[^\\x22\\x5C\\xA\\xD]|" + echar + "|" + uchar_word+")*)\"";
const std::string lang_tag = "@[a-zA-Z]+(?:-[a-zA-Z0-9]+)*";
const std::string suffix_literal = "(\\^\\^" + iri_ref + "|" + lang_tag + ")?";
const std::string string_literal = "(" + string_literal_q + suffix_literal + ")";

const std::string decimal_number = "(^[-+]?[0-9]\\d*(?:\\.\\d+)?$)";

} // namespace

ExprProcessorPersistentData::ExprProcessorPersistentData()
    :
      re_datatype(datatype_regex),
      re_literal(string_literal),
      re_decimal_number(decimal_number),
      mutable_calendar(create_icu_calendar())
{}

ExprDataType ExprProcessorPersistentData::extract_data_type_from_string(
    const std::string &input_string) const {
  pcrecpp::StringPiece re_input(input_string);
  std::string a;
  std::string b;
  bool matched = re_datatype.PartialMatch(re_input, &a, &b);
  if (!matched)
    return ExprDataType::EDT_UNKNOWN;
  if (!a.empty())
    return select_data_type(a);
  else
    return select_data_type(b);
}

ExprDataType ExprProcessorPersistentData::select_data_type(
    const std::string &data_type_string) const {
  if (data_type_string == "integer")
    return ExprDataType::EDT_INTEGER;
  if (data_type_string == "decimal")
    return ExprDataType::EDT_DECIMAL;
  if (data_type_string == "float")
    return ExprDataType::EDT_FLOAT;
  if (data_type_string == "double")
    return ExprDataType::EDT_DOUBLE;
  if (data_type_string == "string")
    return ExprDataType::EDT_STRING;
  if (data_type_string == "boolean")
    return ExprDataType::EDT_BOOLEAN;
  if (data_type_string == "dateTime")
    return ExprDataType::EDT_DATETIME;
  return ExprDataType::EDT_UNKNOWN;
}
std::string ExprProcessorPersistentData::extract_literal_content_from_string(
    const std::string &input_string) const {
  pcrecpp::StringPiece re_input(input_string);
  std::string full_word;
  std::string content;
  std::string metadata;
  bool matched = re_literal.PartialMatch(re_input, &full_word, &content, &metadata);
  if(!matched) return "";
  return content;
}

bool ExprProcessorPersistentData::string_is_numeric(const std::string &input_string) const {
  pcrecpp::StringPiece re_input(input_string);
  std::string full_word;
  return re_decimal_number.FullMatch(re_input, &full_word);
}

icu::TimeZone *ExprProcessorPersistentData::get_utc_timezone()  {
  auto *tz = icu::TimeZone::createDefault();
  tz->setRawOffset(0);
  return tz;
}
std::unique_ptr<icu::Calendar>
ExprProcessorPersistentData::create_icu_calendar() {
  UErrorCode err = U_ZERO_ERROR;
  return std::unique_ptr<icu::Calendar>(icu::Calendar::createInstance(get_utc_timezone(), err));
}
