//
// Created by cristobal on 4/20/21.
//

#ifndef RDFCACHEK2_DATETIMEYEAREVAL_HPP
#define RDFCACHEK2_DATETIMEYEAREVAL_HPP

#include "ExprEval.hpp"
class DateTimeYearEval : public ExprEval {
public:
  int eval_integer(const row_t &row) override;
  void validate() override;
  void init() override;
};

#endif // RDFCACHEK2_DATETIMEYEAREVAL_HPP
