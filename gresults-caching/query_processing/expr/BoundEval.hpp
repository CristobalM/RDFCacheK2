//
// Created by cristobal on 4/20/21.
//

#ifndef RDFCACHEK2_BOUNDEVAL_HPP
#define RDFCACHEK2_BOUNDEVAL_HPP

#include "ExprEval.hpp"
class BoundEval : public ExprEval {
  bool result;

public:
  using ExprEval::ExprEval;
  bool eval_boolean(const row_t &row) override;
  void init() override;
  void validate() override;
  std::unique_ptr<TermResource> eval_resource(const row_t &row) override;
};

#endif // RDFCACHEK2_BOUNDEVAL_HPP
