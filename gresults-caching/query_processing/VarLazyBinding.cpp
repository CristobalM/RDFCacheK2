//
// Created by cristobal on 7/14/21.
//

#include "VarLazyBinding.hpp"
VarLazyBinding::VarLazyBinding(unsigned long var_id,
                               std::unique_ptr<ExprEval> &&expr_eval)
    : var_id(var_id), evaluator(std::move(expr_eval)) {}
unsigned long VarLazyBinding::get_var_id() const { return var_id; }
ExprEval &VarLazyBinding::get_evaluator() { return *evaluator; }
