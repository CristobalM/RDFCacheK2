//
// Created by cristobal on 4/20/21.
//

#ifndef RDFCACHEK2_ISIRIEVAL_HPP
#define RDFCACHEK2_ISIRIEVAL_HPP

#include "ExprEval.hpp"
class IsIRIEval : public ExprEval {
public:
  bool eval_boolean(const row_t &row) const override;
  void init() override;
};

#endif // RDFCACHEK2_ISIRIEVAL_HPP
