//
// Created by cristobal on 4/20/21.
//

#include "StrEncodeForURIEval.hpp"
#include "StringLiteralResource.hpp"
#include <curl/curl.h>

std::unique_ptr<TermResource>
StrEncodeForURIEval::eval_resource(const ExprEval::row_t &row) {
  auto input_resource = children[0]->eval_resource(row);

  if (children_with_error()) {
    return resource_with_error();
  }

  std::string input_str;

  if (input_resource->is_string_literal()) {
    input_str = input_resource->get_literal_string();
  } else if (input_resource->is_string_literal_lang()) {
    input_str = input_resource->get_literal_lang_string();
  } else if (input_resource->is_concrete()) {
    input_str =
        ExprProcessorPersistentData::get().extract_literal_content_from_string(
            input_resource->get_resource().value);
  } else {
    return resource_with_error();
  }

  char *result = curl_easy_escape(nullptr, input_str.c_str(), input_str.size());
  std::string str_result(result);
  curl_free(result);
  return std::make_unique<StringLiteralResource>(std::move(str_result),
                                                 ExprDataType::EDT_UNKNOWN);
}
void StrEncodeForURIEval::validate() {
  ExprEval::validate();
  assert_fsize(1);
}
void StrEncodeForURIEval::init() {
  ExprEval::init();
  add_children();
}
