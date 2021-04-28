

//
// Created by Cristobal Miranda, 2020
//

#ifndef RDFCACHEK2_EXPR_PROCESSOR_PERSISTENT_DATA_HPP
#define RDFCACHEK2_EXPR_PROCESSOR_PERSISTENT_DATA_HPP

#include <pcrecpp.h>
#include <string>

#include <unicode/calendar.h>
#include <unicode/smpdtfmt.h>

#include "DateInfo.hpp"
#include "ExprDataType.hpp"

class ExprProcessorPersistentData {
  const pcrecpp::RE re_datatype;
  const pcrecpp::RE re_literal;
  const pcrecpp::RE re_decimal_number;
  const pcrecpp::RE re_iri;
  std::unique_ptr<icu::Calendar> mutable_calendar;

  static ExprProcessorPersistentData instance;
  ExprProcessorPersistentData();

public:
  ExprDataType
  extract_data_type_from_string(const std::string &input_string) const;

  std::string
  extract_literal_content_from_string(const std::string &input_string) const;

  bool string_is_numeric(const std::string &input_string) const;

  int extract_date_portion(UDate epoch_value,
                           icu::Calendar::EDateFields date_fields);

  DateInfo parse_iso8601(const std::string &input);

  std::string extract_language_tag(const std::string &input_string) const;

  std::string extract_inside_iri(const std::string &input_string) const;

  static ExprProcessorPersistentData &get();

private:
  ExprDataType select_data_type(const std::string &data_type_string) const;

  static icu::TimeZone *get_utc_timezone();
  static std::unique_ptr<icu::Calendar> create_icu_calendar();
};

#endif