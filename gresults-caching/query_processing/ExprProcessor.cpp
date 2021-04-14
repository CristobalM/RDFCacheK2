

#include "ExprProcessor.hpp"

ExprProcessor::ExprProcessor(std::shared_ptr<ResultTable> table,
                             const proto_msg::ExprNode &expr_node)
    : table(std::move(table)), expr_node(expr_node) {}

std::shared_ptr<ResultTable> ExprProcessor::execute() { return table; }