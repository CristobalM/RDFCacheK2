//
// Created by cristobal on 4/20/21.
//

#include "StrLowerCaseEval.hpp"
#include "StringHandlingUtil.hpp"
#include "StringLiteralLangResource.hpp"
#include "StringLiteralResource.hpp"

std::shared_ptr<TermResource>
StrLowerCaseEval::eval_resource(const ExprEval::row_t &row) {
  auto str_resource = children[0]->eval_resource(row);
  if (children_with_error())
    return resource_with_error();
  auto literal_data =
      StringHandlingUtil::extract_literal_data_from_term_resource(
          *str_resource);

  std::for_each(literal_data.value.begin(), literal_data.value.end(),
                [](char &c) { c = std::tolower(c); });

  if (literal_data.error)
    return resource_with_error();
  if (!literal_data.lang_tag.empty()) {
    return std::make_shared<StringLiteralLangResource>(
        std::move(literal_data.value), std::move(literal_data.lang_tag));
  }
  return std::make_shared<StringLiteralResource>(std::move(literal_data.value),
                                                 literal_data.type);
}
void StrLowerCaseEval::validate() {
  ExprEval::validate();
  assert_fsize(1);
}
void StrLowerCaseEval::init() {
  ExprEval::init();
  add_children();
}
