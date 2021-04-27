//
// Created by cristobal on 4/20/21.
//

#include "DigestSHA256Eval.hpp"
#include "StringLiteralResource.hpp"
#include <hashing.hpp>

std::unique_ptr<TermResource>
DigestSHA256Eval::eval_resource(const ExprEval::row_t &row) {
  auto child_resource = children[0]->eval_resource(row);
  std::string result;
  if(child_resource->is_concrete()){
    const auto &concrete_child_resouce = child_resource->get_resource();
    auto literal_content = ExprProcessorPersistentData::get().extract_literal_content_from_string(concrete_child_resouce.value);
    result = sha256_human_readable_lowercase(literal_content);
  }
  else if(child_resource->is_string_literal()){
    result = sha256_human_readable_lowercase(child_resource->get_literal_string());
  }
  else{
    this->with_error = true;
  }
  return std::make_unique<StringLiteralResource>(std::move(result), ExprDataType::EDT_STRING);
}

void DigestSHA256Eval::validate() {
  ExprEval::validate();
  assert_fsize(1);
}
void DigestSHA256Eval::init() {
  ExprEval::init();
  add_children();
}
