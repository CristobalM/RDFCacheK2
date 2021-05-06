
//
// Created by Cristobal Miranda, 2021
//

#ifndef RDFCACHEK2_IS_BLANK_EVAL_HPP
#define RDFCACHEK2_IS_BLANK_EVAL_HPP

#include <memory>

#include "ExprEval.hpp"

#include <request_msg.pb.h>

class IsBlankEval : public ExprEval {
public:
  using ExprEval::ExprEval;
  bool eval_boolean(const row_t &row) override;
  void init() override;
  void validate() override;
  std::unique_ptr<TermResource> eval_resource(const row_t &row) override;
};

#endif