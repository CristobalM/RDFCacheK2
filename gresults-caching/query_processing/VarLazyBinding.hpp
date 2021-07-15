//
// Created by cristobal on 7/14/21.
//

#ifndef RDFCACHEK2_VARLAZYBINDING_HPP
#define RDFCACHEK2_VARLAZYBINDING_HPP

#include <memory>
#include <query_processing/expr/ExprEval.hpp>
class VarLazyBinding {
  unsigned long var_id;
  std::unique_ptr<ExprEval> evaluator;

public:
  VarLazyBinding(unsigned long var_id, std::unique_ptr<ExprEval> &&expr_eval);
  unsigned long get_var_id() const;
  ExprEval &get_evaluator();
};

#endif // RDFCACHEK2_VARLAZYBINDING_HPP
