//
// Created by cristobal on 4/20/21.
//

#ifndef RDFCACHEK2_NUMFLOOREVAL_HPP
#define RDFCACHEK2_NUMFLOOREVAL_HPP

#include "ExprEval.hpp"
class NumFloorEval : public ExprEval {
public:
  std::unique_ptr<TermResource> eval_resource(const row_t &row) override;
  int eval_integer(const row_t &row) override;
  float eval_float(const row_t &row) override;
  double eval_double(const row_t &row) override;
  void validate() override;
  void init() override;
};

#endif // RDFCACHEK2_NUMFLOOREVAL_HPP
