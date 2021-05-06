//
// Created by cristobal on 4/20/21.
//

#include "StrEndsWithEval.hpp"
#include "BooleanResource.hpp"
#include "StringHandlingUtil.hpp"
std::unique_ptr<TermResource>
StrEndsWithEval::eval_resource(const ExprEval::row_t &row) {
  return generate_from_eval_boolean(row);
}

bool StrEndsWithEval::eval_boolean(const ExprEval::row_t &row) {
  auto input_str_resource = children[0]->eval_resource(row);
  auto pattern_resource = children[1]->eval_resource(row);
  if (children_with_error()) {
    return bool_with_error();
  }

  auto input_literal_data =
      StringHandlingUtil::extract_literal_data_from_term_resource(
          *input_str_resource);
  auto pattern_literal_data =
      StringHandlingUtil::extract_literal_data_from_term_resource(
          *pattern_resource);

  if (input_literal_data.error || pattern_literal_data.error) {
    return bool_with_error();
  }

  if ((pattern_literal_data.type != EDT_UNKNOWN &&
       pattern_literal_data.type != EDT_STRING &&
       pattern_literal_data.type != input_literal_data.type) ||

      (!pattern_literal_data.lang_tag.empty() &&
       pattern_literal_data.lang_tag != input_literal_data.lang_tag)) {
    return false;
  }

  return StringHandlingUtil::ends_with(input_literal_data.value,
                                       pattern_literal_data.value);
}
void StrEndsWithEval::validate() {
  ExprEval::validate();
  assert_fsize(2);
}
void StrEndsWithEval::init() {
  ExprEval::init();
  add_children();
}
