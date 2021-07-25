//
// Created by cristobal on 4/20/21.
//

#ifndef RDFCACHEK2_STRREPLACEEVAL_HPP
#define RDFCACHEK2_STRREPLACEEVAL_HPP

#include "ExprEval.hpp"
class StrReplaceEval : public ExprEval {
public:
  using ExprEval::ExprEval;
  std::shared_ptr<TermResource> eval_resource(const row_t &row) override;
  void validate() override;
  void init() override;
};

#endif // RDFCACHEK2_STRREPLACEEVAL_HPP
