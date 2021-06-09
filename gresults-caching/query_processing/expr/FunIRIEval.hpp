//
// Created by cristobal on 4/20/21.
//

#ifndef RDFCACHEK2_FUNIRIEVAL_HPP
#define RDFCACHEK2_FUNIRIEVAL_HPP

#include "ExprEval.hpp"
class FunIRIEval : public ExprEval {
public:
  void init() override;
  std::shared_ptr<TermResource> eval_resource(const row_t &row) override;
  void validate() override;

  using ExprEval::ExprEval;
  std::shared_ptr<TermResource>
  eval_fun_with_term_arguments(TermResource &fun_resource,
                               const ExprEval::row_t &row);
  std::shared_ptr<TermResource> data_type_cast(ExprDataType type,
                                               TermResource &resource);
};

#endif // RDFCACHEK2_FUNIRIEVAL_HPP
