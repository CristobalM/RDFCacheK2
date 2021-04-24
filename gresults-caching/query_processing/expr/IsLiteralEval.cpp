//
// Created by cristobal on 4/20/21.
//

#include "IsLiteralEval.hpp"

bool IsLiteralEval::eval_boolean(const ExprEval::row_t &row) {
  auto resource = children[0]->eval_resource(row);
  if (children_with_error() || !resource->is_concrete()) {
    this->with_error = true;
    return false;
  }
  const auto &concrete_resource = resource->get_resource();
  return concrete_resource.resource_type == RDFResourceType::RDF_TYPE_LITERAL;
}

void IsLiteralEval::init() {
  ExprEval::init();
  add_children();
}
void IsLiteralEval::validate() {
  ExprEval::validate();
  assert_fsize(1);
}
