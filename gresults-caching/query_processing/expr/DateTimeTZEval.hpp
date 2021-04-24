//
// Created by cristobal on 4/20/21.
//

#ifndef RDFCACHEK2_DATETIMETZEVAL_HPP
#define RDFCACHEK2_DATETIMETZEVAL_HPP

#include "ExprEval.hpp"
class DateTimeTZEval : public ExprEval {
public:
  int eval_integer(const row_t &row) override;
  void validate() override;
  void init() override;
};

#endif // RDFCACHEK2_DATETIMETZEVAL_HPP
