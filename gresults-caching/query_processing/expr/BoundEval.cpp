#include <stdexcept>
#include <request_msg.pb.h>

#include "BoundEval.hpp"

bool BoundEval::eval(const std::vector<unsigned long> &) const{
  const proto_msg::ExprNode &node = this->expr_node;
  const EvalData &ed = this->eval_data;
  if(node.expr_case() != proto_msg::ExprNode::kTermNode || node.term_node().term_type() != proto_msg::TermType::VARIABLE)
    throw std::runtime_error("Invalid operand for BOUND(), expected a variable");
  const auto &var = node.term_node().term_value();
  return ed.var_pos_mapping.find(var) != ed.var_pos_mapping.end();
}
