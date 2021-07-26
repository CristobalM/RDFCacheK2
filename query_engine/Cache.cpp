//
// Created by Cristobal Miranda, 2020
//

#include <TimeControl.hpp>
#include <chrono>
#include <query_processing/QueryProcessor.hpp>

#include "Cache.hpp"
#include "CacheReplacementFactory.hpp"

Cache::Cache(std::shared_ptr<PredicatesCacheManager> &cache_manager,
             CacheReplacement::STRATEGY cache_replacement_strategy,
             size_t memory_budget_bytes, std::string temp_files_dir,
             unsigned long timeout_ms)
    : cache_manager(cache_manager),
      cache_replacement(CacheReplacementFactory::select_strategy(
          cache_replacement_strategy, memory_budget_bytes, cache_manager)),
      temp_files_dir(std::move(temp_files_dir)), timeout_ms(timeout_ms) {}

std::shared_ptr<QueryResultIterator>
Cache::run_query(const proto_msg::SparqlTree &query_tree,
                 TimeControl &time_control) {
  ensure_available_predicates(query_tree.root());
  time_control.start_timer();
  return std::make_shared<QueryResultIterator>(
      QueryProcessor(cache_manager, time_control, temp_files_dir)
          .run_query(query_tree.root()));
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
  case proto_msg::SparqlNode::kOrderNode:
    return node_is_valid(node.order_node().node());
  case proto_msg::SparqlNode::kTableNode:
    return true;
  default:
    return false;
  }
}

bool Cache::bgp_node_is_valid(const proto_msg::BGPNode &node) {
  for (int i = 0; i < node.triple_size(); i++) {
    const auto &triple = node.triple(i);
    if (triple.predicate().term_type() != proto_msg::TermType::IRI) {
      std::cerr << "Error, predicate " << triple.predicate().term_value()
                << " is not an IRI " << std::endl;
      return false;
    }
    //    if (!cache_manager->has_predicate(triple.predicate().term_value()))
    //      return false;
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

void Cache::ensure_available_predicates(const proto_msg::SparqlNode &node) {
  switch (node.node_case()) {
  case proto_msg::SparqlNode::kProjectNode:
    ensure_available_predicates(node.project_node().sub_op());
    break;
  case proto_msg::SparqlNode::kLeftJoinNode:
    ensure_available_predicates(node.left_join_node().left_node());
    ensure_available_predicates(node.left_join_node().right_node());
    for (int i = 0; i < node.left_join_node().expr_list_size(); i++) {
      ensure_available_predicates_expr(node.left_join_node().expr_list(i));
    }
    break;
  case proto_msg::SparqlNode::kBgpNode:
    ensure_available_predicates_bgp(node.bgp_node());
    break;
  case proto_msg::SparqlNode::kExprNode:
    ensure_available_predicates_expr(node.expr_node());
    break;
  case proto_msg::SparqlNode::kUnionNode: {
    const auto &union_node = node.union_node();
    for (int i = 0; i < union_node.nodes_list_size(); i++) {
      ensure_available_predicates(union_node.nodes_list(i));
    }
  } break;
  case proto_msg::SparqlNode::kDistinctNode:
    ensure_available_predicates(node.distinct_node().sub_node());
    break;
  case proto_msg::SparqlNode::kOptionalNode:
    ensure_available_predicates(node.optional_node().left_node());
    ensure_available_predicates(node.optional_node().right_node());
    break;
  case proto_msg::SparqlNode::kMinusNode:
    ensure_available_predicates(node.minus_node().left_node());
    ensure_available_predicates(node.minus_node().right_node());
    break;
  case proto_msg::SparqlNode::kFilterNode: {
    const auto &filter_node = node.filter_node();
    ensure_available_predicates(filter_node.node());
    for (int i = 0; i < filter_node.exprs_size(); i++) {
      ensure_available_predicates_expr(filter_node.exprs(i));
    }
  } break;
  case proto_msg::SparqlNode::kExtendNode: {
    const auto &extend_node = node.extend_node();
    ensure_available_predicates(extend_node.node());
    for (int i = 0; i < extend_node.assignments_size(); i++) {
      ensure_available_predicates_expr(extend_node.assignments(i).expr());
    }
  } break;
  case proto_msg::SparqlNode::kSequenceNode: {
    const auto &seq_node = node.sequence_node();
    for (int i = 0; i < seq_node.nodes_size(); i++) {
      ensure_available_predicates(seq_node.nodes(i));
    }
  } break;
  case proto_msg::SparqlNode::kSliceNode: {
    ensure_available_predicates(node.slice_node().node());
  } break;
  case proto_msg::SparqlNode::kOrderNode: {
    ensure_available_predicates(node.order_node().node());
  } break;
  default:
    break;
  }
}
void Cache::ensure_available_predicates_expr(
    const proto_msg::ExprNode &expr_node) {
  switch (expr_node.expr_case()) {
  case proto_msg::ExprNode::kFunctionNode: {
    const auto &fnode = expr_node.function_node();
    for (int i = 0; i < fnode.exprs_size(); i++) {
      ensure_available_predicates_expr(fnode.exprs(i));
    }
  } break;
  case proto_msg::ExprNode::kPatternNode: {
    ensure_available_predicates(expr_node.pattern_node());
  } break;
  default:
    break;
  }
}
void Cache::ensure_available_predicates_bgp(
    const proto_msg::BGPNode &bgp_node) {
  for (int i = 0; i < bgp_node.triple_size(); i++) {
    const auto &triple = bgp_node.triple(i);
    ensure_available_predicate(triple.predicate());
  }
}

void Cache::ensure_available_predicate(const proto_msg::RDFTerm &term) {
  if (term.term_type() == proto_msg::TermType::VARIABLE)
    return;
  cache_manager->ensure_available_predicate(RDFResource(term));
}
unsigned long Cache::get_timeout_ms() { return timeout_ms; }
