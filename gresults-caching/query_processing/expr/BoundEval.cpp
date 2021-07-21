//
// Created by cristobal on 4/20/21.
//

#include "BoundEval.hpp"
#include "query_processing/resources/BooleanResource.hpp"

bool BoundEval::eval_boolean(const ExprEval::row_t &row) {
  if (variable_not_mapped)
    return false;
  auto child_resource = children[0]->eval_resource(row);
  return !child_resource->is_null();
}

void BoundEval::init() {
  ExprEval::init();
  add_children();
  const auto &child_node = expr_node.function_node().exprs(0);
  const auto &var = child_node.term_node().term_value();
  variable_not_mapped = this->eval_data.var_pos_mapping->find(var) ==
                        this->eval_data.var_pos_mapping->end();
}

void BoundEval::validate() {
  ExprEval::validate();
  assert_fun_size(1);
  // const auto &child_node = expr_node.function_node().exprs(0);
  // assert_is_variable(child_node);
}
std::shared_ptr<TermResource>
BoundEval::eval_resource(const ExprEval::row_t &row) {
  return generate_from_eval_boolean(row);
}
