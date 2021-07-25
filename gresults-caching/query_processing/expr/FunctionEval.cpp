//
// Created by cristobal on 4/20/21.
//

#include "FunctionEval.hpp"
void FunctionEval::init() { ExprEval::init(); }
std::shared_ptr<TermResource>
FunctionEval::eval_resource(const ExprEval::row_t &row) {
  auto fun_resource = children[0]->eval_resource(row);
  if (children_with_error()) {
    this->with_error = true;
    return TermResource::null();
  }

  auto result = eval_fun_with_term_arguments(*fun_resource, row);
  return result;
}

void FunctionEval::validate() {
  ExprEval::validate();
  assert_fun_size_gt(0);
  add_children();
}
std::shared_ptr<TermResource>
FunctionEval::eval_fun_with_term_arguments(TermResource &fun_resource,
                                           const ExprEval::row_t &row) {

  if (fun_resource.is_datatype()) {
    if (children.size() != 2) {
      return resource_with_error();
    }
    auto child_res = children[1]->eval_resource(row);
    if (children[1]->has_error()) {
      return resource_with_error();
    }
    return child_res->cast_to(fun_resource.get_datatype());
  }

  return resource_with_error();
}
