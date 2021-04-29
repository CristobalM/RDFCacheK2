//
// Created by cristobal on 4/20/21.
//

#include "StrDataTypeEval.hpp"
#include "StringLiteralResource.hpp"
std::unique_ptr<TermResource>
StrDataTypeEval::eval_resource(const ExprEval::row_t &row) {
  auto input_str_resource = children[0]->eval_resource(row);
  auto data_type_resource = children[1]->eval_resource(row);
  if (children_with_error() || !data_type_resource->is_datatype()) {
    this->with_error = true;
    return TermResource::null();
  }

  if (input_str_resource->is_concrete()) {
    auto lexical_form =
        ExprProcessorPersistentData::get().extract_literal_content_from_string(
            input_str_resource->get_resource().value);
    return std::make_unique<StringLiteralResource>(
        std::move(lexical_form), data_type_resource->get_datatype());
  }
  if (input_str_resource->is_string_literal()) {
    return std::make_unique<StringLiteralResource>(
        std::string(input_str_resource->get_literal_string()),
        data_type_resource->get_datatype());
  }
  if (input_str_resource->is_string_literal_lang()) {
    return std::make_unique<StringLiteralResource>(
        std::string(input_str_resource->get_literal_lang_string()),
        data_type_resource->get_datatype());
  }

  this->with_error = true;
  return TermResource::null();
}
void StrDataTypeEval::validate() {
  ExprEval::validate();
  assert_fsize(2);
}
void StrDataTypeEval::init() {
  ExprEval::init();
  add_children();
}
