//
// Created by cristobal on 4/20/21.
//

#ifndef RDFCACHEK2_UNARYPLUSEVAL_HPP
#define RDFCACHEK2_UNARYPLUSEVAL_HPP

#include "ExprEval.hpp"
class UnaryPlusEval : public ExprEval {
public:
  int eval_integer(const row_t &row) override;
  float eval_float(const row_t &row) override;
  double eval_double(const row_t &row) override;
  void init() override;
  void validate() override;
  std::unique_ptr<TermResource> eval_resource(const row_t &row) override;
};

#endif // RDFCACHEK2_UNARYPLUSEVAL_HPP
