//
// Created by cristobal on 4/20/21.
//

#include "IsIRIEval.hpp"

bool IsIRIEval::eval_boolean(const ExprEval::row_t &row) const {
  const auto &child = *children[0];
  return child.eval_resource(row).resource_type == RDFResourceType::RDF_TYPE_IRI
}
void IsIRIEval::init() {
  ExprEval::init();
  assert_fsize(1);
  const auto &child_node = expr_node.function_node().exprs(0);
  assert_is_rdf_term(child_node);
  add_child(child_node);
}
