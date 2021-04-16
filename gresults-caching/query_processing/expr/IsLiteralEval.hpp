
//
// Created by Cristobal Miranda, 2021
//

#ifndef RDFCACHEK2_IS_LITERAL_EVAL_HPP
#define RDFCACHEK2_IS_LITERAL_EVAL_HPP

#include <memory>

#include "BoolExprEval.hpp"

class IsLiteralEval : public BoolExprEval{
  public:
  bool eval(const std::vector<unsigned long> &row) const override;

};

#endif