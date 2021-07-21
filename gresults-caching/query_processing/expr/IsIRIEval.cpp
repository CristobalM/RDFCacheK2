//
// Created by cristobal on 4/20/21.
//

#include "IsIRIEval.hpp"

bool IsIRIEval::eval_boolean(const ExprEval::row_t &row) {
  auto resource = children[0]->eval_resource(row);
  if (children_with_error() || !resource->is_concrete()) {
    this->with_error = true;
    return false;
  }
  const auto &concrete_resource = resource->get_resource();
  return concrete_resource.resource_type == RDFResourceType::RDF_TYPE_IRI;
}
void IsIRIEval::init() {
  ExprEval::init();
  add_children();
}

void IsIRIEval::validate() {
  ExprEval::validate();
  assert_fun_size(1);
}
std::shared_ptr<TermResource>
IsIRIEval::eval_resource(const ExprEval::row_t &row) {
  return generate_from_eval_boolean(row);
}
