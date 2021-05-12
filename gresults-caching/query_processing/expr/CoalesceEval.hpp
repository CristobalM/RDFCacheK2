//
// Created by cristobal on 4/20/21.
//

#ifndef RDFCACHEK2_COALESCEEVAL_HPP
#define RDFCACHEK2_COALESCEEVAL_HPP

#include "ExprEval.hpp"
#include <memory>

class CoalesceEval : public ExprEval {
public:
  using ExprEval::ExprEval;
  std::shared_ptr<TermResource> eval_resource(const row_t &row) override;
  void validate() override;
  void init() override;
  bool eval_boolean(const row_t &row) override;
};

#endif // RDFCACHEK2_COALESCEEVAL_HPP
