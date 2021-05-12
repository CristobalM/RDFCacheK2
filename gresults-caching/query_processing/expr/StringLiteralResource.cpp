//
// Created by cristobal on 4/25/21.
//

#include "StringLiteralResource.hpp"
#include <query_processing/ExprProcessorPersistentData.hpp>
#include <sstream>
bool StringLiteralResource::is_string_literal() const { return true; }
const std::string &StringLiteralResource::get_literal_string() const {
  return value;
}

ExprDataType StringLiteralResource::get_datatype() const { return data_type; }

StringLiteralResource::StringLiteralResource(std::string &&value,
                                             ExprDataType data_type)
    : value(std::move(value)), data_type(data_type) {}

bool StringLiteralResource::operator==(const TermResource &rhs) const {
  if (!rhs.is_concrete() && !rhs.is_string_literal())
    return false;

  if (rhs.is_string_literal()) {
    return data_type == rhs.get_datatype() && value == rhs.get_literal_string();
  }

  if (rhs.is_concrete()) {
    const auto &rhs_resource = rhs.get_resource();
    if (rhs_resource.resource_type != RDFResourceType::RDF_TYPE_LITERAL)
      return false;
    auto rhs_datatype =
        ExprProcessorPersistentData::get().extract_data_type_from_string(
            rhs_resource.value);
    if (data_type != rhs_datatype)
      return false;
    auto rhs_value =
        ExprProcessorPersistentData::get().extract_literal_content_from_string(
            rhs_resource.value);
    return value == rhs_value;
  }

  return false;
}
bool StringLiteralResource::contains(TermResource &pattern_resource) const {
  if (pattern_resource.is_string_literal()) {
    if (pattern_resource.get_datatype() != data_type)
      return false;
    return value.find(pattern_resource.get_literal_string()) !=
           std::string::npos;
  }
  if (pattern_resource.is_string_literal_lang()) {
    return false;
  }
  if (pattern_resource.is_concrete()) {
    const auto &pattern_full_string = pattern_resource.get_resource().value;
    auto pattern_data_type =
        ExprProcessorPersistentData::get().extract_data_type_from_string(
            pattern_full_string);
    if (pattern_data_type != data_type)
      return false;
    return value.find(pattern_full_string) != std::string::npos;
  }
  return false;
}
StringLiteralResource::StringLiteralResource(std::string &&value)
    : StringLiteralResource(std::move(value), EDT_UNKNOWN) {}
bool StringLiteralResource::is_literal() const { return true; }
RDFResource StringLiteralResource::get_resource_clone() const {
  std::stringstream ss;
  std::string datatype_str;
  switch (data_type) {
  case EDT_UNKNOWN:
    datatype_str = "";
    break;
  case EDT_STRING:
    datatype_str = "^^xsd:string";
    break;
  case EDT_INTEGER:
    datatype_str = "^^xsd:integer";
    break;
  case EDT_DECIMAL:
    datatype_str = "^^xsd:decimal";
    break;
  case EDT_FLOAT:
    datatype_str = "^^xsd:float";
    break;
  case EDT_DOUBLE:
    datatype_str = "^^xsd:double";
    break;

  case EDT_BOOLEAN:
    datatype_str = "^^xsd:boolean";
    break;
  case EDT_DATETIME:
    datatype_str = "^^xsd:dateTime";
    break;
  }
  ss << "\"" << value << "\"" << datatype_str;
  return RDFResource(ss.str(), RDFResourceType::RDF_TYPE_LITERAL);
}
