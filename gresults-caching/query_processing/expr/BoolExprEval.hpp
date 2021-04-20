
//
// Created by Cristobal Miranda, 2021
//

#ifndef RDFCACHEK2_EXPR_EVAL_HPP
#define RDFCACHEK2_EXPR_EVAL_HPP

#include "PredicatesCacheManager.hpp"
#include <unordered_map>
#include <vector>

#include <request_msg.pb.h>

#include "../ExprDataType.hpp"
#include "../ExprProcessorPersistentData.hpp"
#include "EvalData.hpp"

class BoolExprEval {
protected:
  const EvalData &eval_data;
  const proto_msg::ExprNode &expr_node;
  const ExprProcessorPersistentData &persistent_data;

public:
  BoolExprEval(const EvalData &eval_data, const proto_msg::ExprNode &expr_node,
               const ExprProcessorPersistentData &persistent_data);

  virtual ~BoolExprEval();

  virtual bool eval(const std::vector<unsigned long> &row) const = 0;

  template <typename T>
  static std::unique_ptr<BoolExprEval>
  create_unary(const EvalData &eval_data, const proto_msg::ExprNode &expr_node,
               const ExprProcessorPersistentData &persistent_data) {
    return std::make_unique<T>(eval_data, expr_node, persistent_data);
  }
};

#endif
