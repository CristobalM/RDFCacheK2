//
// Created by cristobal on 4/25/21.
//

#include "StringLiteralResource.hpp"
#include <query_processing/ExprProcessorPersistentData.hpp>
bool StringLiteralResource::is_string_literal() const { return true; }
const std::string &StringLiteralResource::get_literal_string() const {
  return value;
}

ExprDataType StringLiteralResource::get_datatype() const { return data_type; }

StringLiteralResource::StringLiteralResource(std::string &&value,
                                             ExprDataType data_type)
    : value(std::move(value)), data_type(data_type) {}

bool StringLiteralResource::operator==(const TermResource &rhs) const {
  if (!rhs.is_concrete() || !rhs.is_string_literal())
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
