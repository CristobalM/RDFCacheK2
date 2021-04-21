

#include "ExprProcessor.hpp"

#include <pcrecpp.h>

ExprProcessor::ExprProcessor(const EvalData &eval_data,
                             const proto_msg::ExprNode &expr_node,
                             const ExprProcessorPersistentData &persistent_data)
    : eval_data(eval_data), expr_node(expr_node),
      persistent_data(persistent_data) {}

std::unique_ptr<ExprEval> ExprProcessor::create_evaluator() {
  auto evaluator =
      ExprEval::create_eval_node(eval_data, persistent_data, expr_node);
  evaluator->init();
  return evaluator;
}
