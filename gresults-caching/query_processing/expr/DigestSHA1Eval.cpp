//
// Created by cristobal on 4/20/21.
//

#include "DigestSHA1Eval.hpp"

#include "query_processing/resources/StringLiteralResource.hpp"

#include <hashing.hpp>

std::shared_ptr<TermResource>
DigestSHA1Eval::eval_resource(const ExprEval::row_t &row) {
  auto child_resource = children[0]->eval_resource(row);
  std::string result;
  if (child_resource->is_concrete()) {
    const auto &concrete_child_resouce = child_resource->get_resource();
    auto literal_content =
        ExprProcessorPersistentData::get().extract_literal_content_from_string(
            concrete_child_resouce.value);
    result = sha1_human_readable_lowercase(literal_content);
  } else if (child_resource->is_string_literal()) {
    result =
        sha1_human_readable_lowercase(child_resource->get_literal_string());
  } else if (child_resource->can_cast_to_literal_string()) {
    result = sha1_human_readable_lowercase(
        child_resource->get_content_string_copy());
  } else {
    this->with_error = true;
  }
  return std::make_shared<StringLiteralResource>(std::move(result));
}

void DigestSHA1Eval::validate() {
  ExprEval::validate();
  assert_fsize(1);
}
void DigestSHA1Eval::init() {
  ExprEval::init();
  add_children();
}
