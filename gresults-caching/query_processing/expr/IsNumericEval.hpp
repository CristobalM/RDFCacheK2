//
// Created by cristobal on 4/20/21.
//

#ifndef RDFCACHEK2_ISNUMERICEVAL_HPP
#define RDFCACHEK2_ISNUMERICEVAL_HPP

#include "ExprEval.hpp"
class IsNumericEval : public ExprEval {
public:
  using ExprEval::ExprEval;
  bool eval_boolean(const row_t &row) override;
  void validate() override;
  void init() override;
  std::shared_ptr<TermResource> eval_resource(const row_t &row) override;
};

#endif // RDFCACHEK2_ISNUMERICEVAL_HPP
