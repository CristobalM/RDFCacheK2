//
// Created by Cristobal Miranda, 2020
//

#include <chrono>
#include <sstream>

#include "Cache.hpp"
#include "CacheReplacementFactory.hpp"

Cache::Cache(std::shared_ptr<PredicatesCacheManager> &cache_manager,
             CacheReplacement::STRATEGY cache_replacement_strategy,
             size_t memory_budget_bytes)
    : cache_manager(cache_manager),
      cache_replacement(CacheReplacementFactory::select_strategy(
          cache_replacement_strategy, memory_budget_bytes, cache_manager)) {}

QueryResult Cache::run_query(proto_msg::SparqlTree const &query_tree) {
  return QueryProcessor(*cache_manager).run_query(query_tree);
}

RDFResource Cache::extract_resource(unsigned long index) const {
  return cache_manager->extract_resource(index);
}

PredicatesCacheManager &Cache::get_pcm() { return *cache_manager; }
bool Cache::query_is_valid(const proto_msg::SparqlTree &tree) {
  return node_is_valid(tree.root());
}
bool Cache::node_is_valid(const proto_msg::SparqlNode &node) {
  switch (node.node_case()) {
  case proto_msg::SparqlNode::kProjectNode:
    return node_is_valid(node.project_node().sub_op());
  case proto_msg::SparqlNode::kLeftJoinNode:
    return node_is_valid(node.left_join_node().left_node()) &&
           node_is_valid(node.left_join_node().right_node());
  case proto_msg::SparqlNode::kBgpNode:
    return bgp_node_is_valid(node.bgp_node());
  case proto_msg::SparqlNode::kUnionNode:
    return union_node_is_valid(node.union_node());
  case proto_msg::SparqlNode::kDistinctNode:
    return node_is_valid(node.distinct_node().sub_node());
  case proto_msg::SparqlNode::kOptionalNode:
    return node_is_valid(node.optional_node().left_node()) &&
           node_is_valid(node.optional_node().right_node());
  case proto_msg::SparqlNode::kMinusNode:
    return node_is_valid(node.minus_node().left_node()) &&
           node_is_valid(node.minus_node().right_node());
  case proto_msg::SparqlNode::kFilterNode:
    return node_is_valid(node.filter_node().node());
  case proto_msg::SparqlNode::kExtendNode:
    return node_is_valid(node.extend_node().node());
  case proto_msg::SparqlNode::kSequenceNode:
    return sequence_is_valid(node.sequence_node());
  case proto_msg::SparqlNode::kSliceNode:
    return node_is_valid(node.slice_node().node());
  default:
    return false;
  }
}

bool Cache::bgp_node_is_valid(const proto_msg::BGPNode &node) {
  for (int i = 0; i < node.triple_size(); i++) {
    const auto &triple = node.triple(i);
    if (triple.predicate().term_type() != proto_msg::TermType::IRI)
      return false;
    if (!cache_manager->has_predicate(triple.predicate().term_value()))
      return false;
  }
  return true;
}
bool Cache::union_node_is_valid(const proto_msg::UnionNode &node) {
  for (int i = 0; i < node.nodes_list_size(); i++) {
    if (!node_is_valid(node.nodes_list(i)))
      return false;
  }
  return true;
}
bool Cache::sequence_is_valid(const proto_msg::SequenceNode &node) {
  for (int i = 0; i < node.nodes_size(); i++) {
    if (!node_is_valid(node.nodes(i)))
      return false;
  }
  return true;
}
