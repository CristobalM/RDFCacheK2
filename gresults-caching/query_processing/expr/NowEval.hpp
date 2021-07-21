//
// Created by cristobal on 6/4/21.
//

#ifndef RDFCACHEK2_NOWEVAL_HPP
#define RDFCACHEK2_NOWEVAL_HPP

#include "ExprEval.hpp"
class NowEval : public ExprEval {
public:
  using ExprEval::ExprEval;
  std::shared_ptr<TermResource> eval_resource(const row_t &row) override;
  DateInfo eval_date_time(const row_t &row) override;
  void validate() override;
  void init() override;
};

#endif // RDFCACHEK2_NOWEVAL_HPP
