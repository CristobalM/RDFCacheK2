//
// Created by cristobal on 4/27/21.
//

#include "StringLiteralLangResource.hpp"
#include <query_processing/ExprProcessorPersistentData.hpp>
StringLiteralLangResource::StringLiteralLangResource(std::string &&value,
                                                     std::string &&lang_tag)
    : value(std::move(value)), lang_tag(std::move(lang_tag)) {}
bool StringLiteralLangResource::is_string_literal_lang() const { return true; }
const std::string &StringLiteralLangResource::get_literal_lang_string() const {
  return value;
}
bool StringLiteralLangResource::operator==(const TermResource &rhs) const {
  if (!rhs.is_concrete() || !rhs.is_string_literal_lang())
    return false;

  if (rhs.is_string_literal_lang()) {
    return lang_tag == rhs.get_lang_tag() &&
           value == rhs.get_literal_lang_string();
  }

  if (rhs.is_concrete()) {
    const auto &rhs_resource = rhs.get_resource();
    if (rhs_resource.resource_type != RDFResourceType::RDF_TYPE_LITERAL)
      return false;
    auto rhs_lang_tag = ExprProcessorPersistentData::get().extract_language_tag(
        rhs_resource.value);
    if (lang_tag != rhs_lang_tag)
      return false;
    auto rhs_value =
        ExprProcessorPersistentData::get().extract_literal_content_from_string(
            rhs_resource.value);
    return value == rhs_value;
  }

  return false;
}
const std::string &StringLiteralLangResource::get_lang_tag() const {
  return lang_tag;
}
