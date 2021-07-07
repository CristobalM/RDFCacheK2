//
// Created by cristobal on 4/20/21.
//

#include "StrLangEval.hpp"
#include "query_processing/resources/StringLiteralLangResource.hpp"
std::shared_ptr<TermResource>
StrLangEval::eval_resource(const ExprEval::row_t &row) {
  auto input_str_resource = children[0]->produce_resource(row);
  auto lang_tag_resource = children[1]->produce_resource(row);

  if (children_with_error()) {
    this->with_error = true;
    return TermResource::null();
  }

  std::string lang_tag_str;

  if (lang_tag_resource->is_concrete()) {
    lang_tag_str =
        ExprProcessorPersistentData::get().extract_literal_content_from_string(
            lang_tag_resource->get_resource().value);

  } else if (lang_tag_resource->is_string_literal_lang()) {
    lang_tag_str = lang_tag_resource->get_literal_lang_string();
  } else if (lang_tag_resource->is_string_literal()) {
    lang_tag_str = lang_tag_resource->get_literal_string();
  } else {
    this->with_error = true;
    return TermResource::null();
  }

  if (input_str_resource->is_concrete()) {
    auto lexical_form =
        ExprProcessorPersistentData::get().extract_literal_content_from_string(
            input_str_resource->get_resource().value);
    return std::make_shared<StringLiteralLangResource>(std::move(lexical_form),
                                                       std::move(lang_tag_str));
  }
  if (input_str_resource->is_string_literal()) {
    return std::make_shared<StringLiteralLangResource>(
        std::string(input_str_resource->get_literal_string()),
        std::move(lang_tag_str));
  }
  if (input_str_resource->is_string_literal_lang()) {
    return std::make_shared<StringLiteralLangResource>(
        std::string(input_str_resource->get_literal_lang_string()),
        std::move(lang_tag_str));
  }

  if (input_str_resource->can_cast_to_literal_string()) {
    return std::make_shared<StringLiteralLangResource>(
        std::string(input_str_resource->get_content_string_copy()),
        std::move(lang_tag_str));
  }

  this->with_error = true;
  return TermResource::null();
}
void StrLangEval::validate() {
  ExprEval::validate();
  assert_fsize(2);
}
void StrLangEval::init() {
  ExprEval::init();
  add_children();
}
