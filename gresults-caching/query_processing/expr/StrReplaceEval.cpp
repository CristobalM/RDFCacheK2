//
// Created by cristobal on 4/20/21.
//

#include "StrReplaceEval.hpp"
#include "query_processing/resources/StringLiteralResource.hpp"
#include "query_processing/utility/StringHandlingUtil.hpp"
std::shared_ptr<TermResource>
StrReplaceEval::eval_resource(const ExprEval::row_t &row) {
  auto text_resource = children[0]->produce_resource(row);
  auto pattern_resource = children[1]->produce_resource(row);
  auto replacement_resource = children[2]->produce_resource(row);
  auto flags_resource = children[3]->produce_resource(row);
  if (children_with_error())
    return resource_with_error();
  auto text_literal_data =
      StringHandlingUtil::extract_literal_data_from_term_resource(
          *text_resource);
  auto pattern_literal_data =
      StringHandlingUtil::extract_literal_data_from_term_resource(
          *pattern_resource);
  auto replacement_literal_data =
      StringHandlingUtil::extract_literal_data_from_term_resource(
          *replacement_resource);
  auto flags_literal_data =
      StringHandlingUtil::extract_literal_data_from_term_resource(
          *flags_resource);
  if (text_literal_data.error || pattern_literal_data.error ||
      replacement_literal_data.error || flags_literal_data.error)
    return resource_with_error();

  int flags_options = StringHandlingUtil::regex_flag_options_from_string(
      flags_literal_data.value);

  pcrecpp::RE_Options options(flags_options);

  pcrecpp::RE re(pattern_literal_data.value, options);

  re.GlobalReplace(replacement_literal_data.value, &text_literal_data.value);

  return std::make_shared<StringLiteralResource>(
      std::move(text_literal_data.value), text_literal_data.type);
}
void StrReplaceEval::validate() {
  ExprEval::validate();
  assert_fsize(4);
}
void StrReplaceEval::init() {
  ExprEval::init();
  add_children();
}
