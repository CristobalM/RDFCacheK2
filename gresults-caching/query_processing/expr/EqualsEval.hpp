//
// Created by cristobal on 4/20/21.
//

#ifndef RDFCACHEK2_EQUALSEVAL_HPP
#define RDFCACHEK2_EQUALSEVAL_HPP

#include "ExprEval.hpp"
class EqualsEval : public ExprEval {
public:
  std::unique_ptr<TermResource> eval_resource(const row_t &row) override;
  void validate() override;
  void init() override;
  bool eval_boolean(const row_t &row) override;
};

#endif // RDFCACHEK2_EQUALSEVAL_HPP
