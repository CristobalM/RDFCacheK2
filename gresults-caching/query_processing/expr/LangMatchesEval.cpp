//
// Created by cristobal on 4/20/21.
//

#include "LangMatchesEval.hpp"
#include "BooleanResource.hpp"

#include <algorithm>

std::unique_ptr<TermResource>
LangMatchesEval::eval_resource(const ExprEval::row_t &row) {
  auto result = eval_boolean(row);
  if (has_error()) {
    return TermResource::null();
  }
  return std::make_unique<BooleanResource>(result);
}
bool LangMatchesEval::eval_boolean(const ExprEval::row_t &row) {
  auto first_resource = children[0]->eval_resource(row);
  auto second_resource = children[1]->eval_resource(row);
  if (children_with_error() || !first_resource->is_concrete() ||
      !second_resource->is_string_literal()) {
    this->with_error = true;
    return false;
  }

  auto resource_language_tag =
      ExprProcessorPersistentData::get().extract_language_tag(
          first_resource->get_resource().value);

  std::for_each(resource_language_tag.begin(), resource_language_tag.end(),
                [](char &c) { c = std::tolower(c); });

  auto query_lang_tag = second_resource->get_literal_string();

  std::for_each(query_lang_tag.begin(), query_lang_tag.end(),
                [](char &c) { c = std::tolower(c); });

  auto is_prefix =
      std::mismatch(query_lang_tag.begin(), query_lang_tag.end(),
                    resource_language_tag.begin(), resource_language_tag.end())
          .first == query_lang_tag.end();
  if (!is_prefix)
    return false;

  if (query_lang_tag.size() == resource_language_tag.size())
    return true;

  // then query_lang_tag.size() < resource_language_tag.size(), because it's a
  // prefix

  return resource_language_tag[query_lang_tag.size()] == '-';
  // if this isn't true, then there is an ALPHA character next and according to
  // the lang-tag matching rule the tags don't match
}
void LangMatchesEval::validate() {
  ExprEval::validate();
  assert_fsize(2);
}
void LangMatchesEval::init() {
  ExprEval::init();
  add_children();
}
