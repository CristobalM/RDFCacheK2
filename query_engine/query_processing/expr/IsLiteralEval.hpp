//
// Created by cristobal on 4/20/21.
//

#ifndef RDFCACHEK2_ISLITERALEVAL_HPP
#define RDFCACHEK2_ISLITERALEVAL_HPP

#include "ExprEval.hpp"
class IsLiteralEval : public ExprEval {
public:
  using ExprEval::ExprEval;
  bool eval_boolean(const row_t &row) override;
  void init() override;
  void validate() override;
  std::shared_ptr<TermResource> eval_resource(const row_t &row) override;
};

#endif // RDFCACHEK2_ISLITERALEVAL_HPP
