//
// Created by cristobal on 4/20/21.
//

#ifndef RDFCACHEK2_DATETIMEDAYEVAL_HPP
#define RDFCACHEK2_DATETIMEDAYEVAL_HPP

#include "ExprEval.hpp"
class DateTimeDayEval : public ExprEval {
public:
  using ExprEval::ExprEval;
  int eval_integer(const row_t &row) override;
  void validate() override;
  void init() override;
};

#endif // RDFCACHEK2_DATETIMEDAYEVAL_HPP
