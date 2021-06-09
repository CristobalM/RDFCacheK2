//
// Created by cristobal on 4/20/21.
//

#include "LangEval.hpp"
#include "query_processing/resources/StringLiteralResource.hpp"
std::shared_ptr<TermResource>
LangEval::eval_resource(const ExprEval::row_t &row) {
  auto child_resource = children[0]->eval_resource(row);
  if (children_with_error() || !child_resource->is_concrete()) {
    this->with_error = true;
    return TermResource::null();
  }
  const auto &concrete_resource = child_resource->get_resource();
  auto language_tag = ExprProcessorPersistentData::get().extract_language_tag(
      concrete_resource.value);
  return std::make_shared<StringLiteralResource>(std::move(language_tag),
                                                 ExprDataType::EDT_STRING);
}

void LangEval::validate() {
  ExprEval::validate();
  assert_fsize(1);
}
void LangEval::init() {
  ExprEval::init();
  add_children();
}
