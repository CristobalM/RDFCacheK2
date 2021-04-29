//
// Created by cristobal on 4/20/21.
//

#ifndef RDFCACHEK2_STRLOWERCASEEVAL_HPP
#define RDFCACHEK2_STRLOWERCASEEVAL_HPP

#include "ExprEval.hpp"
class StrLowerCaseEval : public ExprEval {
public:
  using ExprEval::ExprEval;
  std::unique_ptr<TermResource> eval_resource(const row_t &row) override;
  void validate() override;
  void init() override;
};

#endif // RDFCACHEK2_STRLOWERCASEEVAL_HPP
