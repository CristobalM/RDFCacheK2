//
// Created by cristobal on 4/20/21.
//

#ifndef RDFCACHEK2_DATETIMEHOURSEVAL_HPP
#define RDFCACHEK2_DATETIMEHOURSEVAL_HPP

#include "ExprEval.hpp"
class DateTimeHoursEval : public ExprEval {
public:
  int eval_integer(const row_t &row) override;
  void validate() override;
  void init() override;
};

#endif // RDFCACHEK2_DATETIMEHOURSEVAL_HPP
