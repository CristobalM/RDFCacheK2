//
// Created by cristobal on 4/20/21.
//

#include "LangMatchesEval.hpp"
#include "query_processing/resources/BooleanResource.hpp"
#include "query_processing/utility/StringHandlingUtil.hpp"

#include <algorithm>

std::shared_ptr<TermResource>
LangMatchesEval::eval_resource(const ExprEval::row_t &row) {
  return generate_from_eval_boolean(row);
}
bool LangMatchesEval::eval_boolean(const ExprEval::row_t &row) {
  auto first_resource = children[0]->produce_resource(row);
  auto second_resource = children[1]->produce_resource(row);
  if (children_with_error() || (!first_resource->is_concrete() &&
                                !second_resource->is_string_literal())) {
    this->with_error = true;
    return false;
  }

  auto literal_data =
      StringHandlingUtil::extract_literal_data_from_term_resource(
          *first_resource);

  std::for_each(literal_data.lang_tag.begin(), literal_data.lang_tag.end(),
                [](char &c) { c = std::tolower(c); });

  auto query_lang_tag = second_resource->get_literal_string();

  std::for_each(query_lang_tag.begin(), query_lang_tag.end(),
                [](char &c) { c = std::tolower(c); });

  auto is_prefix =
      std::mismatch(query_lang_tag.begin(), query_lang_tag.end(),
                    literal_data.lang_tag.begin(), literal_data.lang_tag.end())
          .first == query_lang_tag.end();
  if (!is_prefix)
    return false;

  if (query_lang_tag.size() == literal_data.lang_tag.size())
    return true;

  // then query_lang_tag.size() < resource_language_tag.size(), because it's a
  // prefix

  return literal_data.lang_tag[query_lang_tag.size()] == '-';
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
