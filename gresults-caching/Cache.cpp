//
// Created by Cristobal Miranda, 2020
//

#include <stdexcept>
#include <string>
#include <sstream>
#include <unordered_map>
#include <utility>

#include "Cache.hpp"

Cache::Cache(std::unique_ptr<PredicatesCacheManager> &&cache_manager)
    : cache_manager(std::move(cache_manager)) {}

static void process_project_node(const proto_msg::ProjectNode &project_node, ResultTable &result, Cache::cm_t &cm){

}

enum TermType{
  IRI = proto_msg::TermType::IRI,
  BLANK = proto_msg::TermType::BLANK_NODE, 
  LITERAL = proto_msg::TermType::LITERAL, 
  VAR = proto_msg::VARIABLE
};
struct Term{
  TermType type;
  std::string value;
};

struct Triple{
  Term subject, predicate, object;
  Triple(const proto_msg::TripleNode &proto_triple) {
    subject.type = static_cast<TermType>(proto_triple.subject().term_type());
    subject.value = proto_triple.subject().term_value();
    predicate.type = static_cast<TermType>(proto_triple.predicate().term_type());
    predicate.value = proto_triple.predicate().term_value();
    object.type = static_cast<TermType>(proto_triple.object().term_type());
    object.value = proto_triple.object().term_value();
  }
};

struct hash_pair { 
    template <class T1, class T2> 
    size_t operator()(const std::pair<T1, T2>& p) const
    { 
        auto hash1 = std::hash<T1>{}(p.first); 
        auto hash2 = std::hash<T2>{}(p.second); 
        return hash1 ^ hash2; 
    } 
}; 

static std::unordered_map<std::string, K2TreeMixed *> get_k2trees_map_by_predicate_value(const proto_msg::BGPNode &bgp_node, Cache::cm_t &cm){
  std::unordered_map<std::string, K2TreeMixed *> k2trees_map;

  for(size_t i = 0; i < bgp_node.triple_size(); i++){
    const auto &triple = bgp_node.triple().at(i);
    auto predicate_type = triple.predicate().term_type();
    if(predicate_type != proto_msg::TermType::IRI){
      throw std::runtime_error("Operation not supported, predicate is not an IRI");
    }

    const std::string &predicate_value = triple.predicate().term_value();

    if(k2trees_map.find(predicate_value) == k2trees_map.end()){
      k2trees_map[predicate_value] = &cm->get_tree_by_predicate_name(predicate_value);
    }
  }

  return k2trees_map;
}

static 
std::pair<
std::unordered_map<std::string, std::vector<Triple>>,
std::unordered_map<std::pair<std::string, std::string>, std::vector<Triple>, hash_pair>
> group_triple_nodes(const proto_msg::BGPNode &bgp_node){

  std::unordered_map<std::string, std::vector<Triple>> one_var_groups;
  std::unordered_map<std::pair<std::string, std::string>, std::vector<Triple>, hash_pair> two_var_groups;


  for(size_t i = 0; i < bgp_node.triple_size(); i++){
    const auto &triple = bgp_node.triple().at(i);

    if(triple.subject().term_type() == proto_msg::TermType::VARIABLE && 
    triple.object().term_type() == proto_msg::TermType::VARIABLE){
      std::pair<std::string, std::string> keypair;
      if(triple.subject().term_value() < triple.object().term_value()){
        keypair = {triple.subject().term_value(), triple.object().term_value()};
      }
      else{
        keypair = {triple.object().term_value(), triple.subject().term_value()};
      }

      two_var_groups[keypair].push_back(Triple(triple));

    }
    else if(triple.subject().term_type() == proto_msg::TermType::VARIABLE){
      one_var_groups[triple.subject().term_value()].push_back(Triple(triple));
    }
    else if(triple.object().term_type() == proto_msg::TermType::VARIABLE){
      one_var_groups[triple.object().term_value()].push_back(Triple(triple));
    }
    else{
      std::stringstream ss;
      ss << "Found a triple without variable: ";
      ss << "(" 
      << triple.subject().term_value() << ", "
      << triple.predicate().term_value() << ", "
      << triple.object().term_value() 
      << ")";
      
      throw std::runtime_error(ss.str());
    }
  }
  return {one_var_groups, two_var_groups};
}


unsigned long get_index_from_term(const Term &term, Cache::cm_t &cm){
  unsigned long result;
  switch(term.type){
    case TermType::IRI:
    result = cm->get_iri_index(term.value);
    break;
    case TermType::LITERAL:
    result = cm->get_literal_index(term.value);
    break;
    case TermType::BLANK:
    result = cm->get_blank_index(term.value);
    break;
    case TermType::VAR:
    throw std::runtime_error("variables dont have index");
    break;
    default:
    throw std::runtime_error("invalid term of type " + std::to_string(term.type));
  }
  if(result == 0){
    std::stringstream ss;
    ss << "term with value "
    << term.value
    << " and type "
    << term.type
    << "does not exist";
    throw std::runtime_error(ss.str());
  }
  return result;
}


struct VarIndexManager {
  std::unordered_map<std::string, unsigned long> var_indexes;
  unsigned long current_index;
  VarIndexManager() : current_index(0) {}

  void assign_index_if_not_found(const std::string &var_name){
    if(var_indexes.find(var_name) == var_indexes.end()){
      var_indexes[var_name] = current_index++;
    }
  }
};


static std::shared_ptr<ResultTable> join_single_var_group(
  const std::string &var_name,
  const std::vector<Triple> &triples,
  std::unordered_map<std::string, K2TreeMixed *> &k2trees_map,
  Cache::cm_t &cm,
  VarIndexManager &vim){

  std::vector<K2TreeMixed *> join_trees;
  std::vector<struct sip_ipoint> join_coordinates;
  for(const auto &triple : triples){
    join_trees.push_back(k2trees_map[triple.predicate.value]);
    struct sip_ipoint current_sip_ipoint;
    // subjects are columns and objects are rows
    if(triple.subject.type == TermType::VAR){
      current_sip_ipoint.coord = get_index_from_term(triple.object, cm);
      current_sip_ipoint.coord_type = ROW_COORD;
    }
    else{
      current_sip_ipoint.coord = get_index_from_term(triple.subject, cm);
      current_sip_ipoint.coord_type = COLUMN_COORD;
    }
    join_coordinates.push_back(current_sip_ipoint);
  }


  std::vector<unsigned long> join_result = K2TreeMixed::sip_join_k2trees(join_trees, join_coordinates);  

  return std::make_shared<ResultTable>(vim.var_indexes[var_name], std::move(join_result));
}

static std::shared_ptr<ResultTable> join_two_tables_with_trees(
std::shared_ptr<ResultTable> &table_one,
std::shared_ptr<ResultTable> &table_two,
const std::pair<std::pair<std::string, std::string>, std::vector<Triple>> &two_var_group_item,
std::unordered_map<std::string, K2TreeMixed *> &k2trees_map,
Cache::cm_t &cm,
VarIndexManager &vim
){
  std::shared_ptr<ResultTable> smaller, bigger;
  const std::string *smaller_var, *bigger_var;
  if(table_one->rows_size() < table_two->rows_size()){
    smaller = table_one;
    bigger = table_two;

    smaller_var = &two_var_group_item.first.first;
    bigger_var = &two_var_group_item.first.second;
  }
  else{
    smaller = table_two;
    bigger = table_one;

    smaller_var = &two_var_group_item.first.second;
    bigger_var = &two_var_group_item.first.first;
  }

  unsigned long smaller_index = vim.var_indexes[*smaller_var];
  unsigned long smaller_real_index = smaller->get_actual_index(smaller_index);
  

  unsigned long bigger_index = vim.var_indexes[*bigger_var];
  unsigned long bigger_real_index = bigger->get_actual_index(bigger_index);

  for(const auto &row: smaller->data){

    unsigned long current_cord_smaller = row.at(smaller_real_index);
    for(Triple &triple : two_var_group_item.second){
      std::vector<unsigned long> retrieved;
      if(triple.subject.value == *smaller_var){
        retrieved = k2trees_map[triple.predicate.value]->get_row(current_cord_smaller);
      }
      else{
        // is object
        retrieved = k2trees_map[triple.predicate.value]->get_column(current_cord_smaller);
      }
      bigger->left_join_with_vector(bigger_real_index, retrieved, false);

    }

  }
  
  std::unordered_map<std::string, std::shared_ptr<ResultTable>> partial_results;

  partial_results[*smaller_var] = smaller;
  partial_results[*bigger_var] = bigger;

  return cross_product_partial_results(partial_results, cm, vim);
}

static std::shared_ptr<ResultTable> join_table_with_trees(
std::shared_ptr<ResultTable> &table,
const std::pair<std::pair<std::string, std::string>, std::vector<Triple>> &two_var_group_item,
std::unordered_map<std::string, K2TreeMixed *> &k2trees_map,
Cache::cm_t &cm,
VarIndexManager &vim
){

return std::make_shared<ResultTable>(0, std::vector<unsigned long>());
}

static std::shared_ptr<ResultTable> join_two_var_group(
  const std::string &var_one,
  const std::string &var_two,
  const std::vector<Triple> &triples,
  std::unordered_map<std::string, K2TreeMixed *> &k2trees_map,
  Cache::cm_t &cm,
  VarIndexManager &vim
){
  return std::make_shared<ResultTable>(0, std::vector<unsigned long>());
}

static std::shared_ptr<ResultTable> cross_product_partial_results(
  std::unordered_map<std::string, std::shared_ptr<ResultTable>> &partial_results,
  Cache::cm_t &cm, 
  VarIndexManager &vim
){

  return std::make_shared<ResultTable>(0, std::vector<unsigned long>());
}

static std::shared_ptr<ResultTable> process_bgp_node(
  const proto_msg::BGPNode &bgp_node, 
  Cache::cm_t &cm, 
  VarIndexManager &vim){
  std::vector<struct sip_ipoint> join_coordinates(bgp_node.triple_size());

  std::unordered_map<std::string, std::vector<Triple>> one_var_groups;
  std::unordered_map<std::pair<std::string, std::string>, std::vector<Triple>, hash_pair> two_var_groups;

  auto k2trees_map = get_k2trees_map_by_predicate_value(bgp_node, cm);
  auto groups = group_triple_nodes(bgp_node);
  auto &one_var_groups = groups.first;
  auto &two_var_groups = groups.second;

  std::unordered_map<std::string, std::shared_ptr<ResultTable>> partial_results;
  for(const auto &item : one_var_groups){
    vim.assign_index_if_not_found(item.first);
    partial_results[item.first] = std::make_shared<ResultTable>(join_single_var_group(item.first, item.second, k2trees_map, cm, vim));
  }

  for(const auto &item_pair : two_var_groups){
    vim.assign_index_if_not_found(item_pair.first.first);
    vim.assign_index_if_not_found(item_pair.first.second);

    if(partial_results.find(item_pair.first.first) != partial_results.end() &&
    partial_results.find(item_pair.first.second) != partial_results.end()
     ){
      // Both variables are in single var table
      auto &partial_result_one = partial_results[item_pair.first.first];
      auto &partial_result_two = partial_results[item_pair.first.second];
      std::shared_ptr<ResultTable> result;
      if(&partial_result_one != &partial_result_two){
        result = join_two_tables_with_trees(partial_result_one, partial_result_two, item_pair, k2trees_map, cm, vim):
      }
      else{
        result = join_table_with_trees(partial_result_one, item_pair, k2trees_map, cm, vim);
      }

      partial_results[item_pair.first.first] = result;
      partial_results[item_pair.first.second] = result;
    }
    else if(partial_results.find(item_pair.first.first) != partial_results.end()){
      // The first variable only is in a single var table
      auto &partial_result = partial_results[item_pair.first.first];
      partial_result = join_table_with_trees(partial_result, item_pair, k2trees_map, cm, vim);
      partial_results[item_pair.first.second] = partial_result;
    }
    else if(partial_results.find(item_pair.first.second) != partial_results.end()){
      // The second variable only is in a single var table
      auto &partial_result = partial_results[item_pair.first.second];
      partial_result = join_table_with_trees(partial_result, item_pair, k2trees_map, cm, vim);
      partial_results[item_pair.first.first] = partial_result;
    }
    else{
      // None of the variables is in a single var table
      partial_results[item_pair.first.first] = join_two_var_group(item_pair.first.first, item_pair.first.second, item_pair.second, k2trees_map, cm, vim);
      partial_results[item_pair.first.second] = partial_results[item_pair.first.first];
    }
  }

  return cross_product_partial_results(partial_results, cm, vim);
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
