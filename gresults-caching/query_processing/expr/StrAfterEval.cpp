//
// Created by cristobal on 4/20/21.
//

#include "StrAfterEval.hpp"
#include "StringLiteralLangResource.hpp"
#include "StringLiteralResource.hpp"
std::unique_ptr<TermResource>
StrAfterEval::eval_resource(const ExprEval::row_t &row) {
  auto first_resource = children[0]->eval_resource(row);
  auto second_resource = children[1]->eval_resource(row);
  if (children_with_error()) {
    this->with_error = true;
    return TermResource::null();
  }

  std::string first_string;
  std::string first_language_tag;
  ExprDataType first_data_type = EDT_UNKNOWN;
  std::string second_string;
  std::string second_language_tag;
  ExprDataType second_data_type = EDT_UNKNOWN;

  if ((first_resource->is_string_literal() &&
       second_resource->is_string_literal_lang()) ||
      (first_resource->is_string_literal_lang() &&
       second_resource->is_string_literal())) {
    this->with_error = true;
    return TermResource::null();
  }

  if (first_resource->is_concrete()) {
    const std::string &resource_string = first_resource->get_resource().value;

    first_language_tag =
        ExprProcessorPersistentData::get().extract_language_tag(
            resource_string);

    if (first_language_tag.empty())
      first_data_type =
          ExprProcessorPersistentData::get().extract_data_type_from_string(
              resource_string);

    first_string =
        ExprProcessorPersistentData::get().extract_literal_content_from_string(
            resource_string);
  } else if (first_resource->is_string_literal_lang()) {
    first_string = first_resource->get_literal_lang_string();
    first_language_tag = first_resource->get_lang_tag();
  } else if (first_resource->is_string_literal()) {
    first_string = first_resource->get_literal_string();
    first_data_type = first_resource->get_datatype();
  } else {
    this->with_error = true;
    return TermResource::null();
  }

  if (second_resource->is_concrete()) {
    const std::string &resource_string = first_resource->get_resource().value;

    second_language_tag =
        ExprProcessorPersistentData::get().extract_language_tag(
            resource_string);
    second_data_type =
        ExprProcessorPersistentData::get().extract_data_type_from_string(
            resource_string);
    second_string =
        ExprProcessorPersistentData::get().extract_literal_content_from_string(
            resource_string);
  } else if (second_resource->is_string_literal_lang()) {
    second_string = second_resource->get_literal_lang_string();
    second_language_tag = second_resource->get_lang_tag();
  } else if (second_resource->is_string_literal()) {
    second_string = second_resource->get_literal_string();
    second_data_type = second_resource->get_datatype();
  } else {
    this->with_error = true;
    return TermResource::null();
  }

  bool result_is_datatype = false;
  bool result_is_simple = false;
  if (first_data_type != EDT_UNKNOWN && first_data_type == second_data_type) {
    result_is_datatype = true;
  } else if (!first_language_tag.empty() &&
             first_language_tag == second_language_tag) {
  } else if (first_data_type == EDT_UNKNOWN &&
             second_data_type == EDT_UNKNOWN && first_language_tag.empty() &&
             second_language_tag.empty()) {
    result_is_simple = true;
  } else {
    this->with_error = true;
    return TermResource::null();
  }

  if (first_data_type != second_data_type ||
      (first_language_tag.empty() && !second_language_tag.empty()) ||
      (first_language_tag != second_language_tag)) {
    this->with_error = true;
    return TermResource::null();
  }

  auto position = first_string.find(second_string);

  // no match
  if (position == std::string::npos) {
    return std::make_unique<StringLiteralResource>("",
                                                   ExprDataType::EDT_STRING);
  }

  auto resulting_string = first_string.substr(position + second_string.size());

  if (result_is_simple) {
    return std::make_unique<StringLiteralResource>(std::move(resulting_string),
                                                   ExprDataType::EDT_STRING);
  }

  if (result_is_datatype) {
    return std::make_unique<StringLiteralResource>(std::move(resulting_string),
                                                   first_data_type);
  }

  return std::make_unique<StringLiteralLangResource>(
      std::move(resulting_string), std::move(first_language_tag));
}

void StrAfterEval::validate() {
  ExprEval::validate();
  assert_fsize(2);
}
void StrAfterEval::init() {
  ExprEval::init();
  add_children();
}
