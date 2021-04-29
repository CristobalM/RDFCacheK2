//
// Created by cristobal on 4/20/21.
//

#ifndef RDFCACHEK2_STRSUBSTRINGEVAL_HPP
#define RDFCACHEK2_STRSUBSTRINGEVAL_HPP

#include "ExprEval.hpp"
class StrSubstringEval : public ExprEval {
public:
  std::unique_ptr<TermResource> eval_resource(const row_t &row) override;
  void validate() override;
  void init() override;
};

#endif // RDFCACHEK2_STRSUBSTRINGEVAL_HPP
