

//
// Created by Cristobal Miranda, 2020
//
#include "ExprProcessorPersistentData.hpp"

#include <ctime>
#include <sstream>

namespace {
std::string dtype = "(integer|decimal|float|double|string|boolean|dateTime)";
std::string datatype_regex = "\\^\\^(?:<http://www\\.w3\\.org/2001/XMLSchema#" +
                             dtype + ">|xsd:" + dtype + ")";

const std::string hex_letter = "[0-9a-fA-F]";
const std::string uchar_word =
    "\\\\u(?:" + hex_letter + "{8}|" + hex_letter + "{4})";

const std::string iri_letter = "[^\\x00-\\x20<>\"\\{\\}|\\^`\\\\]";
const std::string iri_valid = "<((?:" + iri_letter + "|" + uchar_word + ")*)>";
const std::string iri_ref = "(?:<" + iri_valid + ">)";

const std::string echar = "\\\\[tbnrf\"'\\\\]";
const std::string string_literal_q =
    "\"((?:[^\\x22\\x5C\\xA\\xD]|" + echar + "|" + uchar_word + ")*)\"";
const std::string lang_tag = "@[a-zA-Z]+(?:-[a-zA-Z0-9]+)*";
const std::string suffix_literal = "(\\^\\^" + iri_ref + "|" + lang_tag + ")?";
const std::string string_literal =
    "(" + string_literal_q + suffix_literal + ")";

const std::string decimal_number = "(^[-+]?[0-9]\\d*(?:\\.\\d+)?$)";

} // namespace

ExprProcessorPersistentData::ExprProcessorPersistentData()
    : re_datatype(datatype_regex), re_literal(string_literal),
      re_decimal_number(decimal_number), re_iri(iri_valid),
      mutable_calendar(create_icu_calendar()) {}

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
  bool matched =
      re_literal.PartialMatch(re_input, &full_word, &content, &metadata);
  if (!matched)
    return "";
  return content;
}

bool ExprProcessorPersistentData::string_is_numeric(
    const std::string &input_string) const {
  pcrecpp::StringPiece re_input(input_string);
  std::string full_word;
  return re_decimal_number.FullMatch(re_input, &full_word);
}

icu::TimeZone *ExprProcessorPersistentData::get_utc_timezone() {
  auto *tz = icu::TimeZone::createDefault();
  tz->setRawOffset(0);
  return tz;
}

std::unique_ptr<icu::Calendar>
ExprProcessorPersistentData::create_icu_calendar() {
  UErrorCode err = U_ZERO_ERROR;
  return std::unique_ptr<icu::Calendar>(
      icu::Calendar::createInstance(get_utc_timezone(), err));
}

int ExprProcessorPersistentData::extract_date_portion(
    UDate epoch_value, UCalendarDateFields date_fields) {
  UErrorCode err = U_ZERO_ERROR;
  mutable_calendar->setTime(epoch_value, err);
  return mutable_calendar->get(date_fields, err);
}
DateInfo ExprProcessorPersistentData::parse_iso8601(const std::string &input) {
  static const std::string regex_str(
      "^((\\d{4})-([01]\\d)-([0-3]\\d)T([0-2]\\d):([0-5]\\d):([0-5]\\d)(?:\\.("
      "\\d+))?(?:([+-])([0-2]\\d):?([0-5]\\d)|(Z)))$");
  static const pcrecpp::RE re(regex_str);
  pcrecpp::StringPiece sp(input);
  std::string full_match, year, month, day, hour, minute, second, secfrac,
      offset_sign, offset_hour, offset_minute, utc_z;
  bool matched = re.PartialMatch(sp, &full_match, &year, &month, &day, &hour,
                                 &minute, &second, &secfrac, &offset_sign,
                                 &offset_hour, &offset_minute, &utc_z);
  DateInfo result{};
  if (!matched) {
    result.matched = false;
    return result;
  }
  result.matched = true;
  result.year = std::stoi(year);
  result.month = std::stoi(month);
  result.day = std::stoi(day);
  result.hour = std::stoi(hour);
  result.minute = std::stoi(minute);
  result.second = std::stoi(second);
  if (!secfrac.empty())
    result.second_fraction = std::stoi(secfrac);
  if (!offset_sign.empty()) {
    result.offset_sign = offset_sign == "+" ? 1 : -1;
    result.offset_hour = std::stoi(offset_hour);
    result.offset_minute = std::stoi(offset_minute);
  } else {
    result.offset_sign = 1;
    result.offset_hour = 0;
    result.offset_minute = 0;
  }
  return result;
}

std::string ExprProcessorPersistentData::extract_language_tag(
    const std::string &input_string) const {
  pcrecpp::StringPiece re_input(input_string);
  std::string full_word;
  std::string content;
  std::string metadata;
  bool matched =
      re_literal.PartialMatch(re_input, &full_word, &content, &metadata);
  if (!matched || metadata.empty() || metadata[0] != '@')
    return "";

  return metadata.substr(1, metadata.size() - 1);
}

std::string ExprProcessorPersistentData::extract_inside_iri(
    const std::string &input_string) const {

  pcrecpp::StringPiece piece(input_string);
  std::string inner_string;
  bool matched = re_iri.FullMatch(piece, &inner_string);
  if (!matched)
    return "";
  return inner_string;
}

ExprProcessorPersistentData &ExprProcessorPersistentData::get() {
  return instance;
}

ExprProcessorPersistentData ExprProcessorPersistentData::instance =
    ExprProcessorPersistentData();
DateInfo ExprProcessorPersistentData::get_now_dateinfo() {
  std::time_t now = std::time(0);
  std::tm *now_tm = std::gmtime(&now);
  DateInfo result{};
  result.matched = true;
  result.offset_hour = 0;
  result.offset_minute = 0;
  result.offset_sign = 1;
  result.day = now_tm->tm_mday;
  result.hour = now_tm->tm_hour;
  result.minute = now_tm->tm_min;
  result.second = now_tm->tm_sec;
  result.second_fraction = 0;
  result.month = now_tm->tm_mon;
  result.year = now_tm->tm_year;
  return result;
}

int ExprProcessorPersistentData::normalize_diff_cmp(int diff) {
  if (diff > 0)
    return 1;
  else if (diff < 0)
    return -1;
  return 0;
}

static int count_digits(int input) {
  int total = 0;
  while (input > 0) {
    input /= 10;
    total++;
  }
  return total;
}

static std::string pad_int(int input, int padding) {
  auto digits = count_digits(input);
  auto zeros_to_add = std::max(padding - digits, 0);
  if (zeros_to_add == 0)
    return std::to_string(input);
  return std::string(zeros_to_add, '0') + std::to_string(input);
}

std::string
ExprProcessorPersistentData::date_info_to_iso8601_string(DateInfo date_info) {
  std::stringstream ss;
  ss << date_info.year << "-" << pad_int(date_info.month, 2) << "-"
     << pad_int(date_info.day, 2) << "T" << pad_int(date_info.hour, 2) << ":"
     << pad_int(date_info.minute, 2) << ":" << pad_int(date_info.second, 2);

  if (date_info.second_fraction > 0) {
    ss << "." << date_info.second_fraction;
  }
  if (date_info.offset_minute != 0 || date_info.offset_hour != 0) {
    if (date_info.offset_sign >= 0)
      ss << "+";
    else
      ss << "-";

    ss << pad_int(date_info.offset_hour, 2) << ":"
       << pad_int(date_info.offset_minute, 2);
  } else {
    ss << "Z";
  }
  return ss.str();
}
