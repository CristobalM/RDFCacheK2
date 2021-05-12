//
// Created by cristobal on 4/20/21.
//

#include "RegexEval.hpp"
#include "BooleanResource.hpp"
#include "StringHandlingUtil.hpp"
#include <pcrecpp.h>

std::shared_ptr<TermResource>
RegexEval::eval_resource(const ExprEval::row_t &row) {
  auto result = eval_boolean(row);
  if (has_error()) {
    return TermResource::null();
  }
  return std::make_shared<BooleanResource>(result);
}
bool RegexEval::eval_boolean(const ExprEval::row_t &row) {
  auto resource = children[0]->produce_resource(row);
  auto pattern_resource = children[1]->produce_resource(row);
  auto flags_resource = children[2]->produce_resource(row);

  if (children_with_error()) {
    this->with_error = true;
    return false;
  }

  auto pattern_literal_data =
      StringHandlingUtil::extract_literal_data_from_term_resource(
          *pattern_resource);
  auto resource_literal_data =
      StringHandlingUtil::extract_literal_data_from_term_resource(*resource);
  auto flag_literal_data =
      StringHandlingUtil::extract_literal_data_from_term_resource(
          *flags_resource);

  if (pattern_literal_data.error || resource_literal_data.error ||
      flag_literal_data.error)
    return bool_with_error();

  return match_pattern(resource_literal_data.value, pattern_literal_data.value,
                       flag_literal_data.value);
}

void RegexEval::validate() {
  ExprEval::validate();
  assert_fsize(3);
}
void RegexEval::init() {
  ExprEval::init();
  add_children();
}

bool RegexEval::match_pattern(const std::string &input_string,
                              const std::string &pattern) {
  pcrecpp::StringPiece piece(input_string);
  pcrecpp::RE regex(pattern);
  return regex.FullMatch(piece);
}
bool RegexEval::match_pattern(const std::string &input_string,
                              const std::string &pattern,
                              const std::string &flags) {

  int flags_options = StringHandlingUtil::regex_flag_options_from_string(flags);
  pcrecpp::RE_Options re_options(flags_options);
  pcrecpp::StringPiece piece(input_string);
  pcrecpp::RE regex(pattern, re_options);
  return regex.FullMatch(piece);
}
