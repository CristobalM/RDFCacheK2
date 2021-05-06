
#include "IsBlankEval.hpp"
#include "BooleanResource.hpp"
#include <RDFTriple.hpp>

bool IsBlankEval::eval_boolean(const row_t &row) {
  auto resource = children[0]->eval_resource(row);
  if (children_with_error() || !resource->is_concrete()) {
    this->with_error = true;
    return false;
  }
  const auto &concrete_resource = resource->get_resource();
  return concrete_resource.resource_type == RDFResourceType::RDF_TYPE_BLANK;
}

void IsBlankEval::init() {
  ExprEval::init();
  add_children();
}
void IsBlankEval::validate() {
  ExprEval::validate();
  assert_fsize(1);
}
std::unique_ptr<TermResource>
IsBlankEval::eval_resource(const ExprEval::row_t &row) {
  return generate_from_eval_boolean(row);
}
