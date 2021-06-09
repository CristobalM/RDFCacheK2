//
// Created by cristobal on 4/20/21.
//

#include "StrEval.hpp"
#include "query_processing/resources/StringLiteralResource.hpp"
std::shared_ptr<TermResource>
StrEval::eval_resource(const ExprEval::row_t &row) {
  auto child_resource = children[0]->eval_resource(row);
  if (children_with_error()) {
    this->with_error = true;
    return TermResource::null();
  }

  if (child_resource->is_string_literal()) {
    return child_resource;
  }

  if (child_resource->is_concrete()) {
    const auto &concrete_resource = child_resource->get_resource();
    if (concrete_resource.resource_type != RDFResourceType::RDF_TYPE_IRI) {
      return std::make_shared<StringLiteralResource>(
          std::string(concrete_resource.value), ExprDataType::EDT_STRING);
    }

    return std::make_shared<StringLiteralResource>(
        ExprProcessorPersistentData::get().extract_inside_iri(
            concrete_resource.value),
        ExprDataType::EDT_STRING);
  }

  this->with_error = true;
  return TermResource::null();
}
void StrEval::validate() {
  ExprEval::validate();
  assert_fsize(1);
}
void StrEval::init() {
  ExprEval::init();
  add_children();
}
