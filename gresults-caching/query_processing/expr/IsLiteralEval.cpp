#include <stdexcept>
#include <request_msg.pb.h>

#include "IsLiteralEval.hpp"
#include <RDFTriple.hpp>


bool IsLiteralEval::eval(const std::vector<unsigned long> &row) const{
  const proto_msg::ExprNode &node = this->expr_node;
  const EvalData &ed = this->eval_data;
  if(node.expr_case() != proto_msg::ExprNode::kTermNode || node.term_node().term_type() != proto_msg::TermType::VARIABLE)
    throw std::runtime_error("Invalid operand for IS_LITERAL(), expected a variable");
  const auto &var = node.term_node().term_value();
  if(ed.var_pos_mapping.find(var) == ed.var_pos_mapping.end()) throw std::runtime_error("Variable " + var + " not in table");
  auto pos = ed.var_pos_mapping.at(var);
  auto value_id = row[pos];
  auto resource = ed.cm.extract_resource(value_id);
  return resource.resource_type == RDFResourceType::RDF_TYPE_LITERAL;
}
