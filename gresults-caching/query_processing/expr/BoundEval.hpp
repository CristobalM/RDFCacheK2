//
// Created by cristobal on 4/20/21.
//

#ifndef RDFCACHEK2_BOUNDEVAL_HPP
#define RDFCACHEK2_BOUNDEVAL_HPP

#include "ExprEval.hpp"
class BoundEval : public ExprEval {
  bool result;

public:
  bool eval_boolean(const row_t &row) override;
  void init() override;
  void validate() override;
};

#endif // RDFCACHEK2_BOUNDEVAL_HPP
