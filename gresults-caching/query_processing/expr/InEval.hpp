//
// Created by cristobal on 6/7/21.
//

#ifndef RDFCACHEK2_INEVAL_HPP
#define RDFCACHEK2_INEVAL_HPP

#include "ExprEval.hpp"
class InEval : public ExprEval {
public:
  using ExprEval::ExprEval;
  void init() override;
  std::shared_ptr<TermResource> eval_resource(const row_t &row) override;
  bool eval_boolean(const row_t &row) override;
  void validate() override;
};

#endif // RDFCACHEK2_INEVAL_HPP
