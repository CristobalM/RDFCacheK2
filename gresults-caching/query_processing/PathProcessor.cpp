//
// Created by cristobal on 13-06-21.
//

#include "PathProcessor.hpp"
PathProcessor::PathProcessor(const PredicatesCacheManager &cm,
                             VarIndexManager &vim,
                             NaiveDynamicStringDictionary &extra_str_dict,
                             const proto_msg::TripleWithPath &triple_with_path)
    : cm(cm), vim(vim), extra_str_dict(extra_str_dict),
      triple_with_path(triple_with_path) {}
std::shared_ptr<ResultTable> PathProcessor::execute() {
  result_table = std::make_shared<ResultTable>();
  return process_path_node(triple_with_path.predicate_path_node());
}
std::shared_ptr<ResultTable>
PathProcessor::process_path_node(const proto_msg::PathNode &node) {
  switch (node.node_case()) {
  case proto_msg::PathNode::kLinkPath:
    return process_link_path(node.link_path());
  case proto_msg::PathNode::kInversePath:
    return process_inverse_path(node.inverse_path());
  case proto_msg::PathNode::kSequencePath:
    return process_sequence_path(node.sequence_path());
  case proto_msg::PathNode::kAlternativePath:
    return process_alternative_path(node.alternative_path());
  case proto_msg::PathNode::kZeroOrMorePath:
    return process_zero_or_more_path(node.zero_or_more_path());
  case proto_msg::PathNode::kOneOrMorePath:
    return process_one_or_more_path(node.one_or_more_path());
  case proto_msg::PathNode::kZeroOrOnePath:
    return process_zero_or_one_path(node.zero_or_more_path());
  case proto_msg::PathNode::kNegatedPropertySet:
    return process_negated_property_set(node.negated_property_set());
  case proto_msg::PathNode::kFixedLengthPath:
    return process_fixed_length_parth(node.fixed_length_path());
  case proto_msg::PathNode::kMultiLengthPath:
    return process_multi_length_parth(node.multi_length_path());
  case proto_msg::PathNode::kDistinctPath:
    return process_distinct_path(node.distinct_path());
  default:
    throw std::runtime_error("Invalid path type " +
                             std::to_string(node.node_case()));
  }
}
std::shared_ptr<ResultTable>
PathProcessor::process_link_path(const proto_msg::LinkPath &path) {
  return std::shared_ptr<ResultTable>();
}
std::shared_ptr<ResultTable>
PathProcessor::process_inverse_path(const proto_msg::InversePath &path) {
  return std::shared_ptr<ResultTable>();
}
std::shared_ptr<ResultTable>
PathProcessor::process_sequence_path(const proto_msg::SequencePath &path) {
  return std::shared_ptr<ResultTable>();
}
std::shared_ptr<ResultTable> PathProcessor::process_alternative_path(
    const proto_msg::AlternativePath &path) {
  return std::shared_ptr<ResultTable>();
}
std::shared_ptr<ResultTable> PathProcessor::process_zero_or_more_path(
    const proto_msg::ZeroOrMorePath &path) {
  return std::shared_ptr<ResultTable>();
}
std::shared_ptr<ResultTable>
PathProcessor::process_one_or_more_path(const proto_msg::OneOrMorePath &path) {
  return std::shared_ptr<ResultTable>();
}
std::shared_ptr<ResultTable>
PathProcessor::process_zero_or_one_path(const proto_msg::ZeroOrMorePath &path) {
  return std::shared_ptr<ResultTable>();
}
std::shared_ptr<ResultTable> PathProcessor::process_negated_property_set(
    const proto_msg::NegatedPropertySet &set) {
  return std::shared_ptr<ResultTable>();
}
std::shared_ptr<ResultTable> PathProcessor::process_fixed_length_parth(
    const proto_msg::FixedLengthPath &path) {
  return std::shared_ptr<ResultTable>();
}
std::shared_ptr<ResultTable> PathProcessor::process_multi_length_parth(
    const proto_msg::MultiLengthPath &path) {
  return std::shared_ptr<ResultTable>();
}
std::shared_ptr<ResultTable>
PathProcessor::process_distinct_path(const proto_msg::DistinctPath &path) {
  return std::shared_ptr<ResultTable>();
}
