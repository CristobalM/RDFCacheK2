//
// Created by cristobal on 4/20/21.
//

#ifndef RDFCACHEK2_ISLITERALEVAL_HPP
#define RDFCACHEK2_ISLITERALEVAL_HPP

#include "ExprEval.hpp"
class IsLiteralEval : public ExprEval {
public:
  bool eval_boolean(const row_t &row) override;
  void init() override;
  void validate() override;
};

#endif // RDFCACHEK2_ISLITERALEVAL_HPP
