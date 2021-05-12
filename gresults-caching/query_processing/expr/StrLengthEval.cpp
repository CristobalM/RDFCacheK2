//
// Created by cristobal on 4/20/21.
//

#include "StrLengthEval.hpp"
#include "IntegerResource.hpp"
#include "StringHandlingUtil.hpp"

std::shared_ptr<TermResource>
StrLengthEval::eval_resource(const ExprEval::row_t &row) {
  int result = eval_integer(row);
  if (has_error())
    return resource_with_error();
  return std::make_shared<IntegerResource>(result);
}

int StrLengthEval::eval_integer(const ExprEval::row_t &row) {
  auto str_resource = children[0]->eval_resource(row);
  if (children_with_error())
    return integer_with_error();
  auto literal_data =
      StringHandlingUtil::extract_literal_data_from_term_resource(
          *str_resource);
  if (literal_data.error)
    return integer_with_error();
  return static_cast<int>(literal_data.value.size());
}

void StrLengthEval::validate() {
  ExprEval::validate();
  assert_fsize(1);
}
void StrLengthEval::init() {
  ExprEval::init();
  add_children();
}
