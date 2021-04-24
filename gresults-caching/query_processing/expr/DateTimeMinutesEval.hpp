//
// Created by cristobal on 4/20/21.
//

#ifndef RDFCACHEK2_DATETIMEMINUTESEVAL_HPP
#define RDFCACHEK2_DATETIMEMINUTESEVAL_HPP

#include "ExprEval.hpp"
class DateTimeMinutesEval : ExprEval {
  int eval_integer(const row_t &row) override;
  void validate() override;
  void init() override;
};

#endif // RDFCACHEK2_DATETIMEMINUTESEVAL_HPP
