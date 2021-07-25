//
// Created by cristobal on 6/7/21.
//

#ifndef RDFCACHEK2_RANDEVAL_HPP
#define RDFCACHEK2_RANDEVAL_HPP

#include <chrono>
#include <random>

#include "ExprEval.hpp"
class RandEval : public ExprEval {
  std::mt19937_64 rng;
  std::uniform_real_distribution<double> uniform_distribution;

public:
  using ExprEval::ExprEval;
  void init() override;
  std::shared_ptr<TermResource> eval_resource(const row_t &row) override;
  double eval_double(const row_t &row) override;
  void validate() override;
};

#endif // RDFCACHEK2_RANDEVAL_HPP
