//
// Created by cristobal on 4/20/21.
//

#ifndef RDFCACHEK2_DIGESTMD5EVAL_HPP
#define RDFCACHEK2_DIGESTMD5EVAL_HPP

#include "ExprEval.hpp"
class DigestMD5Eval : public ExprEval {
public:
  std::unique_ptr<TermResource> eval_resource(const row_t &row) override;
  void validate() override;
  void init() override;
};

#endif // RDFCACHEK2_DIGESTMD5EVAL_HPP
