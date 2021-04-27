//
// Created by cristobal on 4/20/21.
//

#ifndef RDFCACHEK2_DATETIMETZEVAL_HPP
#define RDFCACHEK2_DATETIMETZEVAL_HPP

#include "ExprEval.hpp"
class DateTimeTZEval : public ExprEval {
public:
  void validate() override;
  void init() override;
  std::unique_ptr<TermResource> eval_resource(const row_t &row) override;
};

#endif // RDFCACHEK2_DATETIMETZEVAL_HPP
