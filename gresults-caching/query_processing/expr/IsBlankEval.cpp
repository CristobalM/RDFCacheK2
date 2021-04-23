#include <request_msg.pb.h>
#include <stdexcept>

#include "IsBlankEval.hpp"
#include <RDFTriple.hpp>

bool IsBlankEval::eval_boolean(const row_t &row)  {
  auto resource = children[0]->eval_resource(row);
  if(!resource->is_concrete()){
    this->with_error = true;
    return false;
  }

  

  return children[0]->eval_resource(row).resource_type ==
         RDFResourceType::RDF_TYPE_BLANK;
}

void IsBlankEval::init() {
  ExprEval::init();
  assert_fsize(1);
  const auto &child_node = expr_node.function_node().exprs(0);
  assert_is_rdf_term(child_node);
  add_child(child_node);
}
