//
// Created by cristobal on 5/5/21.
//

#ifndef RDFCACHEK2_DATETIMESECONDSEVAL_HPP
#define RDFCACHEK2_DATETIMESECONDSEVAL_HPP

#include "ExprEval.hpp"
class DateTimeSecondsEval : public ExprEval {
public:
  using ExprEval::ExprEval;
  int eval_integer(const row_t &row) override;
  void validate() override;
  void init() override;
  std::unique_ptr<TermResource> eval_resource(const row_t &row) override;
};

#endif // RDFCACHEK2_DATETIMESECONDEVAL_HPP
