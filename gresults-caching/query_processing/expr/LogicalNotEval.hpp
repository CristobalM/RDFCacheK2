
//
// Created by Cristobal Miranda, 2021
//

#ifndef RDFCACHEK2_LOGICAL_NOT_EVAL_HPP
#define RDFCACHEK2_LOGICAL_NOT_EVAL_HPP

#include <memory>

#include "BoolExprEval.hpp"

class LogicalNotEval : public BoolExprEval {
  std::unique_ptr<BoolExprEval> child;

public:
  LogicalNotEval(const EvalData &eval_data,
                 const proto_msg::ExprNode &expr_node,
                 const ExprProcessorPersistentData &persistent_data);

  bool eval(const std::vector<unsigned long> &row) const override;
};

#endif