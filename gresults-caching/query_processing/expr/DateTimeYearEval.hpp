//
// Created by cristobal on 4/20/21.
//

#ifndef RDFCACHEK2_DATETIMEYEAREVAL_HPP
#define RDFCACHEK2_DATETIMEYEAREVAL_HPP

#include "ExprEval.hpp"
class DateTimeYearEval : public ExprEval {
public:
  using ExprEval::ExprEval;
  int eval_integer(const row_t &row) override;
  void validate() override;
  void init() override;
  std::shared_ptr<TermResource> eval_resource(const row_t &row) override;
};

#endif // RDFCACHEK2_DATETIMEYEAREVAL_HPP
