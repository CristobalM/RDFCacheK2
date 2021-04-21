#include <request_msg.pb.h>
#include <stdexcept>

#include "IsBlankEval.hpp"
#include <RDFTriple.hpp>

bool IsBlankEval::eval_boolean(const row_t &row) const {
  const auto &child = *children[0];
  return child.eval_resource(row).resource_type ==
         RDFResourceType::RDF_TYPE_BLANK;
}

void IsBlankEval::init() {
  ExprEval::init();
  assert_fsize(1);
  const auto &child_node = expr_node.function_node().exprs(0);
  assert_is_rdf_term(child_node);
  add_child(child_node);
}
