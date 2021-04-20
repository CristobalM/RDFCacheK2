#include <request_msg.pb.h>
#include <stdexcept>

#include "LogicalNotEval.hpp"
#include <RDFTriple.hpp>

#include "../ExprProcessor.hpp"

LogicalNotEval::LogicalNotEval(
    const EvalData &eval_data, const proto_msg::ExprNode &expr_node,
    const ExprProcessorPersistentData &persistent_data)
    : BoolExprEval(eval_data, expr_node, persistent_data),
      child(
          ExprProcessor(this->eval_data, this->expr_node).create_evaluator()) {}

bool LogicalNotEval::eval(const std::vector<unsigned long> &row) const {
  return !child->eval(row);
}
