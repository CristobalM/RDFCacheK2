

//
// Created by Cristobal Miranda, 2020
//
#include "ExprProcessorPersistentData.hpp"

namespace {
std::string dtype = "(integer|decimal|float|double|string|boolean|dateTime)";
std::string datatype_regex = "\\^\\^(?:<http://www\\.w3\\.org/2001/XMLSchema#" +
                             dtype + ">|xsd:" + dtype + ")";
} // namespace

ExprProcessorPersistentData::ExprProcessorPersistentData()
    : re_datatype(datatype_regex) {}

ExprDataType ExprProcessorPersistentData::extract_data_type_from_string(
    const std::string &input_string) const {
  pcrecpp::StringPiece re_input(input_string);
  std::string a;
  std::string b;
  bool matched = re_datatype.PartialMatch(re_input, &a, &b);
  if (!matched)
    return ExprDataType::EDT_UNKNOWN;
  if (a.size() > 0)
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
    return ExprDataType::EDT_BOOOLEAN;
  if (data_type_string == "dateTime")
    return ExprDataType::EDT_DATETIME;
  return ExprDataType::EDT_UNKNOWN;
}
