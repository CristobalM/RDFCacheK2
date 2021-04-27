//
// Created by cristobal on 4/20/21.
//

#include "RegexEval.hpp"
#include "BooleanResource.hpp"
#include <pcrecpp.h>

std::unique_ptr<TermResource>
RegexEval::eval_resource(const ExprEval::row_t &row) {
  auto result = eval_boolean(row);
  if(has_error()){
    return TermResource::null();
  }
  return std::make_unique<BooleanResource>(result);
}
bool RegexEval::eval_boolean(const ExprEval::row_t &row) {
  auto resource = children[0]->eval_resource(row);
  auto pattern_resource = children[1]->eval_resource(row);
  if(children_with_error()){
    this->with_error = true;
    return false;
  }
  std::string pattern_string;
  if(pattern_resource->is_concrete()){
    pattern_string = ExprProcessorPersistentData::get().extract_literal_content_from_string(pattern_resource->get_resource().value);
  }
  else if(pattern_resource->is_string_literal()){
    pattern_string = pattern_resource->get_literal_string();
  }
  else{
    this->with_error = true;
    return false;
  }


  if(resource->is_concrete()){
    auto literal_content = ExprProcessorPersistentData::get().extract_literal_content_from_string(resource->get_resource().value);
    return match_pattern(literal_content, pattern_string);
  }

  if(resource->is_string_literal()){
    return match_pattern(resource->get_literal_string(), pattern_string);
  }

  this->with_error = true;
  return false;
}

void RegexEval::validate() {
  ExprEval::validate();
  assert_fsize(3); // ignore the third for now, it's a flag
}
void RegexEval::init() {
  ExprEval::init();
  add_children(2);
}

bool RegexEval::match_pattern(const std::string &input_string,
                              const std::string &pattern) {
  pcrecpp::StringPiece piece(input_string);
  pcrecpp::RE regex(pattern);
  return regex.FullMatch(piece);
}
