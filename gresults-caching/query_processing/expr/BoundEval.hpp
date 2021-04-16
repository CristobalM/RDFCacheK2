
//
// Created by Cristobal Miranda, 2021
//

#ifndef RDFCACHEK2_BOUND_EVAL_HPP
#define RDFCACHEK2_BOUND_EVAL_HPP

#include <memory>

#include "BoolExprEval.hpp"

class BoundEval : public BoolExprEval{
  public:
  bool eval(const std::vector<unsigned long> &row) const override;

};

#endif