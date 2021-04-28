
//
// Created by Cristobal Miranda, 2020
//

#ifndef RDFCACHEK2_EXPR_PROCESSOR_HPP
#define RDFCACHEK2_EXPR_PROCESSOR_HPP

#include <memory>
#include <string>
#include <unordered_map>

#include <request_msg.pb.h>

#include "ExprProcessorPersistentData.hpp"
#include "PredicatesCacheManager.hpp"
#include "ResultTable.hpp"
#include "VarIndexManager.hpp"
#include "expr/EvalData.hpp"
#include "expr/ExprEval.hpp"

class ExprProcessor {
  using row_t = std::vector<unsigned long>;

  const EvalData &eval_data;
  const proto_msg::ExprNode &expr_node;

public:
  ExprProcessor(const EvalData &eval_data,
                const proto_msg::ExprNode &expr_node);

  std::unique_ptr<ExprEval> create_evaluator();

private:
  template <typename T> std::unique_ptr<ExprEval> create_expr_eval() {
    return std::make_unique<T>(eval_data, expr_node);
  }
};

#endif /* RDFCACHEK2_EXPR_PROCESSOR_HPP */