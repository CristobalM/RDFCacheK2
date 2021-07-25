

//
// Created by Cristobal Miranda, 2020
//

#ifndef RDFCACHEK2_PARSING_UTILS_HPP
#define RDFCACHEK2_PARSING_UTILS_HPP

#include <memory>
#include <string>

#include <pcrecpp.h>

#include <unicode/calendar.h>
#include <unicode/smpdtfmt.h>

#include "DateInfo.hpp"
#include "ExprDataType.hpp"

class ParsingUtils {

public:
  static ExprDataType
  extract_data_type_from_string(const std::string &input_string);

  static std::string
  extract_literal_content_from_string(const std::string &input_string);

  static bool string_is_numeric(const std::string &input_string);

  static DateInfo parse_iso8601(const std::string &input);

  static std::string extract_language_tag(const std::string &input_string);

  static std::string extract_inside_iri(const std::string &input_string);

  static DateInfo get_now_dateinfo();

  static int normalize_diff_cmp(int diff);

  static std::string date_info_to_iso8601_string(DateInfo date_info);

private:
  static ExprDataType select_data_type(const std::string &data_type_string);
};

#endif