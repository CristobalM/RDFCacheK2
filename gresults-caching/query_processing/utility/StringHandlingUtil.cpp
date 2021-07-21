//
// Created by cristobal on 4/29/21.
//

#include "StringHandlingUtil.hpp"
#include <query_processing/ParsingUtils.hpp>
StringLiteralData StringHandlingUtil::extract_literal_data_from_term_resource(
    const TermResource &term_resource) {
  StringLiteralData result{};
  result.type = EDT_UNKNOWN;
  if (term_resource.is_concrete()) {
    result =
        extract_literal_data_from_rdf_resource(term_resource.get_resource());
  } else if (term_resource.is_string_literal_lang()) {
    result.value = term_resource.get_literal_lang_string();
    result.lang_tag = term_resource.get_lang_tag();
  } else if (term_resource.is_string_literal()) {
    result.value = term_resource.get_literal_string();
    result.type = term_resource.get_datatype();
  } else if (term_resource.can_cast_to_literal_string()) {
    result.value = term_resource.get_content_string_copy();
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
StringLiteralData StringHandlingUtil::extract_literal_data_from_rdf_resource(
    const RDFResource &rdf_resource) {
  return extract_literal_data_from_string(rdf_resource.value);
}

bool StringHandlingUtil::starts_with(const std::string &input,
                                     const std::string &pattern) {
  if (pattern.size() > input.size())
    return false;
  for (size_t i = 0; i < pattern.size(); i++) {
    if (input[i] != pattern[i])
      return false;
  }
  return true;
}
bool StringHandlingUtil::ends_with(const std::string &input,
                                   const std::string &pattern) {
  if (pattern.size() > input.size())
    return false;
  for (size_t input_i = input.size() - 1, pattern_i = pattern.size() - 1;
       input_i >= input.size() - pattern.size(); input_i--, pattern_i--) {
    if (input[input_i] != pattern[pattern_i])
      return false;
  }
  return true;
}
StringLiteralData StringHandlingUtil::extract_literal_data_from_string(
    const std::string &input_string) {
  StringLiteralData result;
  result.value =
      ParsingUtils::extract_literal_content_from_string(input_string);
  result.lang_tag = ParsingUtils::extract_language_tag(input_string);
  result.type = ParsingUtils::extract_data_type_from_string(input_string);
  return result;
}
