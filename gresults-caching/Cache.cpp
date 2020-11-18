//
// Created by Cristobal Miranda, 2020
//

#include <stdexcept>
#include <string>
#include <unordered_map>

#include "Cache.hpp"

Cache::Cache(std::unique_ptr<PredicatesCacheManager> &&cache_manager)
    : cache_manager(std::move(cache_manager)) {}

static void process_project_node(const proto_msg::ProjectNode &project_node, ResultTable &result, Cache::cm_t &cm){

}

static void process_bgp_node(const proto_msg::BGPNode &bgp_node, ResultTable &result, Cache::cm_t &cm){
  std::unordered_map<std::string, K2TreeMixed *> k2trees_map;
  std::vector<K2TreeMixed *> trees(bgp_node.triple_size);
  std::vector<struct sip_ipoint> join_coordinates(bgp_node.triple_size);

  for(size_t i = 0; i < bgp_node.triple_size; i++){
    const auto &triple = bgp_node.triple().at(i);
    auto predicate_type = triple.predicate().term_type();
    if(predicate_type != proto_msg::TermType::IRI){
      throw std::runtime_error("Operation not supported, predicate is not an IRI");
    }

    const std::string &predicate_value = triple.predicate().term_value();

    if(k2trees_map.find(predicate_value) == k2trees_map.end()){
      k2trees_map[predicate_value] = &cm->get_tree_by_predicate_name(predicate_value);
    }

    trees[i] = k2trees_map[predicate_value];

    if(triple.subject().term_type() == proto_msg::TermType::VARIABLE){
      
    }
    

    // K2TreeMixed &predicate_tree = *k2trees_map[predicate_value];
  }
}

static void process_expr_node(const proto_msg::ExprNode &expr_node, ResultTable &result, Cache::cm_t &cm){
}

static void process_left_join_node(const proto_msg::LeftJoinNode &left_join_node, ResultTable &result, Cache::cm_t &cm){
}

static void process_triple_node(const proto_msg::TripleNode &triple_node, ResultTable &result, Cache::cm_t &cm){
}

void process_node(const proto_msg::SparqlNode &node, ResultTable &result, Cache::cm_t &cm){
  switch(node.node_case()){
    case proto_msg::SparqlNode::NodeCase::kProjectNode:
    process_project_node(node.project_node(), result, cm);
    break;
    case proto_msg::SparqlNode::NodeCase::kBgpNode:
    process_bgp_node(node.bgp_node(), result, cm);
    break;
    case proto_msg::SparqlNode::NodeCase::kExprNode:
    process_expr_node(node.expr_node(), result, cm);
    break;
    case proto_msg::SparqlNode::NodeCase::kLeftJoinNode:
    process_left_join_node(node.left_join_node(), result, cm);
    break;
    case proto_msg::SparqlNode::NodeCase::kTripleNode:
    process_triple_node(node.triple_node(), result, cm);
    break;
    case proto_msg::SparqlNode::NodeCase::NODE_NOT_SET:
    default:
    throw std::runtime_error("Unknown SparqlNode : " + std::to_string(node.node_case()));
    break;
  }
}

QueryResult Cache::run_query(const proto_msg::SparqlTree &query_tree) {
  ResultTable result;
  process_node(query_tree.root(), result, cache_manager);
  return QueryResult(std::move(result));
}
