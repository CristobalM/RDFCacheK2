//
// Created by cristobal on 4/20/21.
//

#include "ConditionalEval.hpp"
std::unique_ptr<TermResource>
ConditionalEval::eval_resource(const ExprEval::row_t &row) {
  auto is_true = children[0]->eval_boolean(row);
  if (children[0]->has_error()) {
    this->with_error = true;
    return TermResource::null();
  }

  if (is_true) {
    auto result = children[1]->eval_resource(row);
    if (children[1]->has_error()) {
      this->with_error = true;
      return TermResource::null();
    }
    return result;
  } else {
    auto result = children[2]->eval_resource(row);
    if (children[2]->has_error()) {
      this->with_error = true;
      return TermResource::null();
    }
    return result;
  }
}

void ConditionalEval::validate() {
  ExprEval::validate();
  assert_fsize(3);
  assert_is_function(expr_node);
}
void ConditionalEval::init() {
  ExprEval::init();
  const auto &function_node = expr_node.function_node();
  add_children();
}
