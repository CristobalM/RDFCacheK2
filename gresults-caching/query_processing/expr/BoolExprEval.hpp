
//
// Created by Cristobal Miranda, 2021
//

#ifndef RDFCACHEK2_EXPR_EVAL_HPP
#define RDFCACHEK2_EXPR_EVAL_HPP

#include <vector>
#include <unordered_map>
#include "PredicatesCacheManager.hpp"

#include <request_msg.pb.h>

#include "EvalData.hpp"


class BoolExprEval{
  protected:
  const EvalData &eval_data;
  const proto_msg::ExprNode &expr_node;
  public:
  BoolExprEval(const EvalData &eval_data, const proto_msg::ExprNode &expr_node) : eval_data(eval_data), expr_node(expr_node) {}
  
  virtual bool eval(const std::vector<unsigned long> &row) const = 0;


template<typename T>
static std::unique_ptr<BoolExprEval> create_unary(const EvalData &eval_data, const proto_msg::ExprNode &expr_node){
  return std::make_unique<T>(eval_data, expr_node);
}

};

#endif