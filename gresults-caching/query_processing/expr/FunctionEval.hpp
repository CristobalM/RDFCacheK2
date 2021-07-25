//
// Created by cristobal on 4/20/21.
//

#ifndef RDFCACHEK2_FUNCTIONEVAL_HPP
#define RDFCACHEK2_FUNCTIONEVAL_HPP

#include "ExprEval.hpp"
class FunctionEval : public ExprEval {
public:
  using ExprEval::ExprEval;

  void init() override;
  std::shared_ptr<TermResource> eval_resource(const row_t &row) override;
  void validate() override;

  std::shared_ptr<TermResource>
  eval_fun_with_term_arguments(TermResource &fun_resource,
                               const ExprEval::row_t &row);
};

#endif // RDFCACHEK2_FUNCTIONEVAL_HPP
