//
// Created by cristobal on 4/20/21.
//

#ifndef RDFCACHEK2_COALESCEEVAL_HPP
#define RDFCACHEK2_COALESCEEVAL_HPP

#include "ExprEval.hpp"
class CoalesceEval : public ExprEval {
public:
  RDFResource eval_resource(const row_t &row) override;
  void validate() override;
  void init() override;
};

#endif // RDFCACHEK2_COALESCEEVAL_HPP
