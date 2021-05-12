//
// Created by cristobal on 4/20/21.
//

#ifndef RDFCACHEK2_NOTEQUALSEVAL_HPP
#define RDFCACHEK2_NOTEQUALSEVAL_HPP

#include "ExprEval.hpp"
class NotEqualsEval : public ExprEval {
public:
  using ExprEval::ExprEval;
  std::shared_ptr<TermResource> eval_resource(const row_t &row) override;
  void validate() override;
  void init() override;
  bool eval_boolean(const row_t &row) override;
};

#endif // RDFCACHEK2_NOTEQUALSEVAL_HPP
