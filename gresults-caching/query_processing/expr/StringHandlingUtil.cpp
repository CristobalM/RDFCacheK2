//
// Created by cristobal on 4/29/21.
//

#include "StringHandlingUtil.hpp"
#include <query_processing/ExprProcessorPersistentData.hpp>
StringLiteralData StringHandlingUtil::extract_literal_data_from_term_resource(
    const TermResource &term_resource) {
  StringLiteralData result{};
  result.type = EDT_UNKNOWN;
  if (term_resource.is_concrete()) {
    const auto &resource_str = term_resource.get_resource().value;
    result.value =
        ExprProcessorPersistentData::get().extract_literal_content_from_string(
            resource_str);
    result.lang_tag =
        ExprProcessorPersistentData::get().extract_language_tag(resource_str);
    result.type =
        ExprProcessorPersistentData::get().extract_data_type_from_string(
            resource_str);
  } else if (term_resource.is_string_literal_lang()) {
    result.value = term_resource.get_literal_lang_string();
    result.lang_tag = term_resource.get_lang_tag();
  } else if (term_resource.is_string_literal()) {
    result.value = term_resource.get_literal_string();
    result.type = term_resource.get_datatype();
  } else {
    result.error = true;
  }
  return result;
}

int StringHandlingUtil::regex_flag_options_from_string(
    const std::string &input_flags) {
  int options = 0;
  for (char c : input_flags) {
    switch (c) {
    case 'i':
      options |= PCRE_CASELESS;
      break;
    case 'm':
      options |= PCRE_MULTILINE;
      break;
    case 's':
      options |= PCRE_DOTALL;
      break;
    default:
      break;
    }
  }
  return options;
}
