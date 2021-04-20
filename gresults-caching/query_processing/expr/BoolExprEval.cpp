
//
// Created by Cristobal Miranda, 2021
//

#include "BoolExprEval.hpp"

#include <pcrecpp.h>

BoolExprEval::BoolExprEval(const EvalData &eval_data,
                           const proto_msg::ExprNode &expr_node,
                           const ExprProcessorPersistentData &persistent_data)
    : eval_data(eval_data), expr_node(expr_node),
      persistent_data(persistent_data) {}

BoolExprEval::~BoolExprEval() {}