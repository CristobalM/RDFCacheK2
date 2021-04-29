//
// Created by cristobal on 4/20/21.
//

#include "StrSubstringEval.hpp"
#include "StringHandlingUtil.hpp"
#include "StringLiteralLangResource.hpp"
#include "StringLiteralResource.hpp"
std::unique_ptr<TermResource>
StrSubstringEval::eval_resource(const ExprEval::row_t &row) {
  auto source_resource = children[0]->eval_resource(row);
  int starting_loc = children[1]->eval_integer(row);
  int length = children[2]->eval_integer(row);
  if (children_with_error())
    return resource_with_error();
  auto source_literal_data =
      StringHandlingUtil::extract_literal_data_from_term_resource(
          *source_resource);
  if (source_literal_data.error)
    return resource_with_error();
  auto substr = source_literal_data.value.substr(starting_loc, length);
  if (!source_literal_data.lang_tag.empty()) {
    return std::make_unique<StringLiteralLangResource>(
        std::move(substr), std::move(source_literal_data.lang_tag));
  }
  return std::make_unique<StringLiteralResource>(std::move(substr),
                                                 source_literal_data.type);
}
void StrSubstringEval::validate() {
  ExprEval::validate();
  assert_fsize(3);
}
void StrSubstringEval::init() {
  ExprEval::init();
  add_children();
}
