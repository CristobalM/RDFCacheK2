
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
  [[nodiscard]] bool eval_boolean(const row_t &row) const override;
  void init() override;
};

#endif