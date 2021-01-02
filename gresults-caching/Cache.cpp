//
// Created by Cristobal Miranda, 2020
//

#include <set>
#include <sstream>
#include <stdexcept>
#include <string>
#include <unordered_map>
#include <unordered_set>
#include <utility>

#include "Cache.hpp"

#include "VarIndexManager.hpp"
#include "CacheReplacementFactory.hpp"

Cache::Cache(std::unique_ptr<PredicatesCacheManager> &&cache_manager, CacheReplacement::STRATEGY cache_replacement_strategy, size_t memory_budget_bytes)
    : cache_manager(std::move(cache_manager)), cache_replacement(CacheReplacementFactory::select_strategy(cache_replacement_strategy, memory_budget_bytes)) {}

enum TermType {
  IRI = proto_msg::TermType::IRI,
  BLANK = proto_msg::TermType::BLANK_NODE,
  LITERAL = proto_msg::TermType::LITERAL,
  VAR = proto_msg::VARIABLE
};
struct Term {
  TermType type;
  std::string value;
};

struct Triple {
  Term subject, predicate, object;
  Triple(const proto_msg::TripleNode &proto_triple) {
    subject.type = static_cast<TermType>(proto_triple.subject().term_type());
    subject.value = proto_triple.subject().term_value();
    predicate.type =
        static_cast<TermType>(proto_triple.predicate().term_type());
    predicate.value = proto_triple.predicate().term_value();
    object.type = static_cast<TermType>(proto_triple.object().term_type());
    object.value = proto_triple.object().term_value();
  }
};

struct hash_pair {
  template <class T1, class T2>
  size_t operator()(const std::pair<T1, T2> &p) const {
    auto hash1 = std::hash<T1>{}(p.first);
    auto hash2 = std::hash<T2>{}(p.second);
    return hash1 ^ hash2;
  }
};

// prototypes

std::shared_ptr<ResultTable>
process_join_node(const proto_msg::SparqlNode &join_node, Cache::cm_t &cm,
                  VarIndexManager &vim);

std::shared_ptr<ResultTable>
process_bgp_node(const proto_msg::BGPNode &bgp_node, Cache::cm_t &cm,
                 VarIndexManager &vim);

std::unordered_map<std::string, K2TreeMixed *>
get_k2trees_map_by_predicate_value(const proto_msg::BGPNode &bgp_node,
                                   Cache::cm_t &cm);

std::pair<std::unordered_map<std::string, std::vector<Triple>>,
          std::unordered_map<std::pair<std::string, std::string>,
                             std::vector<Triple>, hash_pair>>
group_triple_nodes(const proto_msg::BGPNode &bgp_node);

unsigned long get_index_from_term(const Term &term, Cache::cm_t &cm);

std::shared_ptr<ResultTable> join_single_var_group(
    const std::string &var_name, const std::vector<Triple> &triples,
    std::unordered_map<std::string, K2TreeMixed *> &k2trees_map,
    Cache::cm_t &cm, VarIndexManager &vim);

std::shared_ptr<ResultTable> cross_product_partial_results(
    std::unordered_map<std::string, std::shared_ptr<ResultTable>>
        &partial_results);

std::shared_ptr<ResultTable> cross_product_partial_results(
    std::unordered_map<std::string, std::shared_ptr<ResultTable>>
        &partial_results);

std::shared_ptr<ResultTable> join_table_with_trees_by_two_var(
    std::shared_ptr<ResultTable> &table,
    const std::pair<std::pair<std::string, std::string>, std::vector<Triple>>
        &two_var_group_item,
    std::unordered_map<std::string, K2TreeMixed *> &k2trees_map,
    VarIndexManager &vim);

std::shared_ptr<ResultTable>
join_two_var_group(const std::string &var_one, const std::string &var_two,
                   const std::vector<Triple> &triples,
                   std::unordered_map<std::string, K2TreeMixed *> &k2trees_map,
                   VarIndexManager &vim);

std::shared_ptr<ResultTable>
join_two_var_group_sip(const std::string &var_one, const std::string &var_two,
                   const std::vector<Triple> &triples,
                   std::unordered_map<std::string, K2TreeMixed *> &k2trees_map,
                   VarIndexManager &vim);

void process_expr_node(const proto_msg::ExprNode &,
                       std::shared_ptr<ResultTable> &, Cache::cm_t &);

std::shared_ptr<ResultTable>
process_left_join_node(const proto_msg::LeftJoinNode &left_join_node,
                       Cache::cm_t &cm, VarIndexManager &vim);

std::shared_ptr<ResultTable>
process_project_node(const proto_msg::ProjectNode &project_node,
                     Cache::cm_t &cm, VarIndexManager &vim);

void process_triple_node(const proto_msg::TripleNode &,
                         std::shared_ptr<ResultTable> &, Cache::cm_t &);

void remove_extra_vars_from_table(std::shared_ptr<ResultTable> input_table,
                                  const std::vector<std::string> &vars,
                                  VarIndexManager &vim);
// definitions

std::unordered_map<std::string, K2TreeMixed *>
get_k2trees_map_by_predicate_value(const proto_msg::BGPNode &bgp_node,
                                   Cache::cm_t &cm) {
  std::unordered_map<std::string, K2TreeMixed *> k2trees_map;

  for (int i = 0; i < bgp_node.triple_size(); i++) {
    const auto &triple = bgp_node.triple().at(i);
    auto predicate_type = triple.predicate().term_type();
    if (predicate_type != proto_msg::TermType::IRI) {
      throw std::runtime_error(
          "Operation not supported, predicate is not an IRI");
    }

    const std::string &predicate_value = triple.predicate().term_value();

    if (k2trees_map.find(predicate_value) == k2trees_map.end()) {
      k2trees_map[predicate_value] =
          &cm->get_tree_by_predicate_name(predicate_value);
    }
  }

  return k2trees_map;
}

std::pair<std::unordered_map<std::string, std::vector<Triple>>,
          std::unordered_map<std::pair<std::string, std::string>,
                             std::vector<Triple>, hash_pair>>
group_triple_nodes(const proto_msg::BGPNode &bgp_node) {

  std::unordered_map<std::string, std::vector<Triple>> one_var_groups;
  std::unordered_map<std::pair<std::string, std::string>, std::vector<Triple>,
                     hash_pair>
      two_var_groups;

  for (int i = 0; i < bgp_node.triple_size(); i++) {
    const auto &triple = bgp_node.triple().at(i);

    if (triple.subject().term_type() == proto_msg::TermType::VARIABLE &&
        triple.object().term_type() == proto_msg::TermType::VARIABLE) {
      std::pair<std::string, std::string> keypair;
      if (triple.subject().term_value() < triple.object().term_value()) {
        keypair = {triple.subject().term_value(), triple.object().term_value()};
      } else {
        keypair = {triple.object().term_value(), triple.subject().term_value()};
      }

      two_var_groups[keypair].push_back(Triple(triple));

    } else if (triple.subject().term_type() == proto_msg::TermType::VARIABLE) {
      one_var_groups[triple.subject().term_value()].push_back(Triple(triple));
    } else if (triple.object().term_type() == proto_msg::TermType::VARIABLE) {
      one_var_groups[triple.object().term_value()].push_back(Triple(triple));
    } else {
      std::stringstream ss;
      ss << "Found a triple without variable: ";
      ss << "(" << triple.subject().term_value() << ", "
         << triple.predicate().term_value() << ", "
         << triple.object().term_value() << ")";

      throw std::runtime_error(ss.str());
    }
  }
  return {one_var_groups, two_var_groups};
}

unsigned long get_index_from_term(const Term &term, Cache::cm_t &cm) {
  unsigned long result;
  switch (term.type) {
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
    throw std::runtime_error("invalid term of type " +
                             std::to_string(term.type));
  }
  if (result == 0) {
    std::stringstream ss;
    ss << "term with value " << term.value << " and type " << term.type
       << "does not exist";
    throw std::runtime_error(ss.str());
  }
  return result;
}

std::shared_ptr<ResultTable> join_single_var_group(
    const std::string &var_name, const std::vector<Triple> &triples,
    std::unordered_map<std::string, K2TreeMixed *> &k2trees_map,
    Cache::cm_t &cm, VarIndexManager &vim) {

  std::vector<K2TreeMixed *> join_trees;
  std::vector<struct sip_ipoint> join_coordinates;
  for (const auto &triple : triples) {
    join_trees.push_back(k2trees_map[triple.predicate.value]);
    struct sip_ipoint current_sip_ipoint;
    // subjects are columns and objects are rows
    if (triple.subject.type == TermType::VAR) {
      current_sip_ipoint.coord = get_index_from_term(triple.object, cm);
      current_sip_ipoint.coord_type = ROW_COORD;
    } else {
      current_sip_ipoint.coord = get_index_from_term(triple.subject, cm);
      current_sip_ipoint.coord_type = COLUMN_COORD;
    }
    join_coordinates.push_back(current_sip_ipoint);
  }

  std::vector<unsigned long> join_result =
      K2TreeMixed::sip_join_k2trees(join_trees, join_coordinates);

  return std::make_shared<ResultTable>(vim.var_indexes[var_name],
                                       std::move(join_result));
}

std::shared_ptr<ResultTable> cross_product_partial_results(
    std::unordered_map<std::string, std::shared_ptr<ResultTable>>
        &partial_results) {
  if (partial_results.size() == 0)
    throw std::runtime_error("cross_product_partial_results: no results");
  if (partial_results.size() == 1)
    return partial_results[partial_results.begin()->first];

  auto it_left = partial_results.begin();
  auto left_table = it_left->second;
  auto second = std::next(it_left);

  std::vector<std::pair<std::string, std::shared_ptr<ResultTable>>>
      plain_partial_results;
  for (auto it = second; it != partial_results.end(); it++)
    plain_partial_results.push_back({it->first, it->second});

  std::set<ResultTable *> done;

  done.insert(it_left->second.get());

  for (const auto &pair : plain_partial_results) {
    const auto &var_name = pair.first;
    auto table = pair.second;
    if (done.find(table.get()) != done.end()) {
      continue;
    }

    for (auto right_header : table->headers) {
      left_table->headers.push_back(right_header);
    }

    for (auto &left_row : left_table->data) {
      for (auto &right_row : table->data) {
        for (auto right_val : right_row) {
          left_row.push_back(right_val);
        }
      }
    }

    partial_results[var_name] = left_table;
    done.insert(table.get());
  }

  return left_table;
}

std::shared_ptr<ResultTable> join_two_tables_with_trees(
    std::shared_ptr<ResultTable> &table_one,
    std::shared_ptr<ResultTable> &table_two,
    const std::pair<std::pair<std::string, std::string>, std::vector<Triple>>
        &two_var_group_item,
    std::unordered_map<std::string, K2TreeMixed *> &k2trees_map,
    VarIndexManager &vim) {
  std::shared_ptr<ResultTable> smaller, bigger;
  const std::string *smaller_var, *bigger_var;
  if (table_one->rows_size() < table_two->rows_size()) {
    smaller = table_one;
    bigger = table_two;

    smaller_var = &two_var_group_item.first.first;
    bigger_var = &two_var_group_item.first.second;
  } else {
    smaller = table_two;
    bigger = table_one;

    smaller_var = &two_var_group_item.first.second;
    bigger_var = &two_var_group_item.first.first;
  }

  unsigned long smaller_index = vim.var_indexes[*smaller_var];
  unsigned long smaller_real_index = smaller->get_actual_index(smaller_index);

  unsigned long bigger_index = vim.var_indexes[*bigger_var];
  unsigned long bigger_real_index = bigger->get_actual_index(bigger_index);

  for (const auto &row : smaller->data) {

    unsigned long current_cord_smaller = row.at(smaller_real_index);
    for (const Triple &triple : two_var_group_item.second) {
      std::vector<unsigned long> retrieved;
      if (triple.subject.value == *smaller_var) {
        retrieved =
            k2trees_map[triple.predicate.value]->get_row(current_cord_smaller);
      } else {
        // is object
        retrieved = k2trees_map[triple.predicate.value]->get_column(
            current_cord_smaller);
      }
      bigger->left_join_with_vector(bigger_real_index, retrieved, false);
    }
  }

  std::unordered_map<std::string, std::shared_ptr<ResultTable>> partial_results;

  partial_results[*smaller_var] = smaller;
  partial_results[*bigger_var] = bigger;

  return cross_product_partial_results(partial_results);
}

std::shared_ptr<ResultTable> join_table_with_trees_by_two_var(
    std::shared_ptr<ResultTable> &table,
    const std::pair<std::pair<std::string, std::string>, std::vector<Triple>>
        &two_var_group_item,
    std::unordered_map<std::string, K2TreeMixed *> &k2trees_map,
    VarIndexManager &vim) {

  const std::string &left_string = two_var_group_item.first.first;
  const std::string &right_string = two_var_group_item.first.second;

  unsigned long left_index = vim.var_indexes[left_string];
  unsigned long left_real_index = table->get_actual_index(left_index);
  unsigned long right_index = vim.var_indexes[right_string];
  unsigned long right_real_index = table->get_actual_index(right_index);

  for (auto it = table->data.begin(); it != table->data.end(); it++) {
    auto &row = *it;
    unsigned long left_value = row[left_real_index];
    unsigned long right_value = row[right_real_index];
    for (const auto &triple : two_var_group_item.second) {
      K2TreeMixed &k2tree = *k2trees_map[triple.predicate.value];
      bool keep_value;
      if (triple.subject.value == left_string) {
        keep_value = k2tree.has(left_value, right_value);
      } else {
        keep_value = k2tree.has(right_value, left_value);
      }

      if (!keep_value) {
        auto del_it = it;
        it--;
        table->data.erase(del_it);
        break;
      }
    }
  }

  return table;
}

std::shared_ptr<ResultTable> join_table_with_trees_by_one_var(
    std::shared_ptr<ResultTable> &table, const std::string &table_var,
    const std::string &other_var,
    const std::pair<std::pair<std::string, std::string>, std::vector<Triple>>
        &two_var_group_item,
    std::unordered_map<std::string, K2TreeMixed *> &k2trees_map,
    VarIndexManager &vim) {

  unsigned long table_var_index = vim.var_indexes[table_var];
  unsigned long real_table_var_index = table->get_actual_index(table_var_index);

  vim.assign_index_if_not_found(other_var);
  unsigned long other_var_index = vim.var_indexes[other_var];

  table->headers.push_back(other_var_index);

  for (const auto &triple : two_var_group_item.second) {
    auto &k2 = *k2trees_map[triple.predicate.value];
    std::vector<std::pair<unsigned long, unsigned long>> pairs;
    k2.scan_points(
        [](unsigned long col, unsigned long row, void *report_state) {
          auto &pairs = *reinterpret_cast<
              std::vector<std::pair<unsigned long, unsigned long>> *>(
              report_state);
          pairs.push_back({col, row});
        },
        &pairs);
  }

  for (auto it = table->data.begin(); it != table->data.end();) {
    auto &row = *it;
    unsigned long current_table_value = row[real_table_var_index];
    std::vector<K2TreeMixed *> join_trees;
    std::vector<struct sip_ipoint> join_coordinates;
    for (const auto &triple : two_var_group_item.second) {
      join_trees.push_back(k2trees_map[triple.predicate.value]);
      struct sip_ipoint join_point;
      join_point.coord = current_table_value;
      if (triple.subject.value == table_var) {
        join_point.coord_type = coord_t::COLUMN_COORD;
      } else {
        join_point.coord_type = coord_t::ROW_COORD;
      }
      join_coordinates.push_back(join_point);
    }
    auto join_result =
        K2TreeMixed::sip_join_k2trees(join_trees, join_coordinates);
    auto next_it = std::next(it);

    if (join_result.size() == 0) {
      table->data.erase(it);
    } else {
      for (size_t curr_val_i = 0; curr_val_i < join_result.size();
           curr_val_i++) {
        auto value = join_result[curr_val_i];
        auto &current_row = *it;
        auto row_copy = current_row;
        current_row.push_back(value);
        if (curr_val_i < join_result.size() - 1)
          it = table->data.insert(next_it, std::move(row_copy));
      }
    }

    it = next_it;
  }

  return table;
}

struct ChainedInput {
  ResultTable &result_table;
  const std::vector<Triple> &triples;
  const std::unordered_map<std::string, K2TreeMixed *> &k2trees_map;
  const std::vector<coord_t> &coord_types;
  ChainedInput(
    ResultTable &result_table, 
    const std::vector<Triple> &triples, 
    const std::unordered_map<std::string, K2TreeMixed *> &k2trees_map,
    const std::vector<coord_t> &coord_types
    ):
  result_table(result_table), 
  triples(triples),
  k2trees_map(k2trees_map),
  coord_types(coord_types)
  {}
};

std::shared_ptr<ResultTable> join_two_var_group(
    const std::string &var_one, const std::string &var_two,
    const std::vector<Triple> &triples,
    std::unordered_map<std::string, K2TreeMixed *> &k2trees_map,
    VarIndexManager &vim) {

  if (triples.empty())
    throw std::runtime_error("join_two_var_group: no triples");

  const auto &first_triple = triples.at(0);
  K2TreeMixed &first_k2tree = *k2trees_map[first_triple.predicate.value];

  auto result = std::make_shared<ResultTable>(
    std::vector<unsigned long>(
    {
      vim.var_indexes[var_one], 
      vim.var_indexes[var_two]
    })
  );


  std::vector<coord_t> coord_types;

  for(auto &triple: triples){
    coord_t ctype;
    if(triple.subject.value == var_one){
      ctype = COLUMN_COORD;
    }
    else{
      ctype = ROW_COORD;
    }
    coord_types.push_back(ctype);
  }

  ChainedInput chained_input(*result, triples, k2trees_map, coord_types);



  first_k2tree.scan_points([](unsigned long col, unsigned long row, void *report_state){
    auto &chained_input = *reinterpret_cast<ChainedInput *>(report_state);
    auto &result_table = chained_input.result_table;
    const auto &triples = chained_input.triples;
    const auto &k2trees_map = chained_input.k2trees_map;
    const auto &coord_types = chained_input.coord_types;
    bool valid = true;
    for(size_t i = 1; i < triples.size(); i++){
      auto &triple = triples[i];
      auto &k2tree = *k2trees_map.at(triple.predicate.value);
      if(coord_types[i] == COLUMN_COORD){
        valid = k2tree.has(col, row);
      }
      else{
        valid = k2tree.has(row, col);
      }
      if(!valid) break;
    }

    if(valid){
      if(coord_types[0] == COLUMN_COORD)
      result_table.data.push_back(std::vector<unsigned long>({col, row}));
      else
      result_table.data.push_back(std::vector<unsigned long>({row, col}));
    }
  }, &chained_input);
  

  return result;

}

std::shared_ptr<ResultTable>
join_two_var_group_sip(const std::string &var_one, const std::string &var_two,
                   const std::vector<Triple> &triples,
                   std::unordered_map<std::string, K2TreeMixed *> &k2trees_map,
                   VarIndexManager &vim) {

  if (triples.empty())
    throw std::runtime_error("join_two_var_group: no triples");

  const auto &first_triple = triples.at(0);

  K2TreeMixed &first_k2tree = *k2trees_map[first_triple.predicate.value];

  std::set<unsigned long> set_of_coords;

  if (first_triple.subject.value == var_one) {

    first_k2tree.scan_points(
        [](unsigned long col, unsigned long, void *report_state) {
          auto &set_of_coords =
              *reinterpret_cast<std::set<unsigned long> *>(report_state);
          set_of_coords.insert(col);
        },
        &set_of_coords);
  } else {

    first_k2tree.scan_points(
        [](unsigned long, unsigned long row, void *report_state) {
          auto &set_of_coords =
              *reinterpret_cast<std::set<unsigned long> *>(report_state);
          set_of_coords.insert(row);
        },
        &set_of_coords);
  }

  std::vector<unsigned long> first_k2tree_coords(set_of_coords.begin(),
                                                 set_of_coords.end());

  std::vector<K2TreeMixed *> join_trees;

  for (const auto &triple : triples) {
    auto *k2tree_ptr = k2trees_map[triple.predicate.value];
    join_trees.push_back(k2tree_ptr);
  }

  unsigned long left_index = vim.var_indexes[var_one];
  unsigned long right_index = vim.var_indexes[var_two];

  auto table = std::make_shared<ResultTable>(
      std::vector<unsigned long>({left_index, right_index}));
  for (unsigned long coord : first_k2tree_coords) {
    std::vector<struct sip_ipoint> join_coordinates;
    for (const auto &triple : triples) {
      struct sip_ipoint point;
      point.coord = coord;
      if (triple.subject.value == var_one) {
        point.coord_type = COLUMN_COORD;
      } else {
        point.coord_type = ROW_COORD;
      }
      join_coordinates.push_back(point);
    }
    auto band = K2TreeMixed::sip_join_k2trees(join_trees, join_coordinates);
    for (auto band_item : band) {
      table->add_row({coord, band_item});
    }
  }

  return table;
}

std::shared_ptr<ResultTable>
process_join_node(const proto_msg::SparqlNode &join_node, Cache::cm_t &cm,
                  VarIndexManager &vim) {

  std::shared_ptr<ResultTable> result_table;

  switch (join_node.node_case()) {
  case proto_msg::SparqlNode::NodeCase::kBgpNode:
    result_table = process_bgp_node(join_node.bgp_node(), cm, vim);
    break;
  case proto_msg::SparqlNode::NodeCase::kLeftJoinNode:
    result_table = process_left_join_node(join_node.left_join_node(), cm, vim);
    break;
  default:
    throw std::runtime_error("Unsupported nodetype on process_join_node: " +
                             std::to_string(join_node.node_case()));
  }

  return result_table;
}

std::shared_ptr<ResultTable>
process_bgp_node(const proto_msg::BGPNode &bgp_node, Cache::cm_t &cm,
                 VarIndexManager &vim) {

  auto k2trees_map = get_k2trees_map_by_predicate_value(bgp_node, cm);
  auto groups = group_triple_nodes(bgp_node);
  auto &one_var_groups = groups.first;
  auto &two_var_groups = groups.second;

  std::unordered_map<std::string, std::shared_ptr<ResultTable>> partial_results;
  for (const auto &item : one_var_groups) {
    vim.assign_index_if_not_found(item.first);
    partial_results[item.first] =
        join_single_var_group(item.first, item.second, k2trees_map, cm, vim);
  }

  for (const auto &item_pair : two_var_groups) {
    vim.assign_index_if_not_found(item_pair.first.first);
    vim.assign_index_if_not_found(item_pair.first.second);

    if (partial_results.find(item_pair.first.first) != partial_results.end() &&
        partial_results.find(item_pair.first.second) != partial_results.end()) {
      // Both variables are in single var table
      auto &partial_result_one = partial_results[item_pair.first.first];
      auto &partial_result_two = partial_results[item_pair.first.second];
      std::shared_ptr<ResultTable> result;
      if (&partial_result_one != &partial_result_two) {
        result =
            join_two_tables_with_trees(partial_result_one, partial_result_two,
                                       item_pair, k2trees_map, vim);
      } else {
        result = join_table_with_trees_by_two_var(partial_result_one, item_pair,
                                                  k2trees_map, vim);
      }

      partial_results[item_pair.first.first] = result;
      partial_results[item_pair.first.second] = result;
    } else if (partial_results.find(item_pair.first.first) !=
               partial_results.end()) {
      // The first variable only is in a single var table
      auto &partial_result = partial_results[item_pair.first.first];
      partial_result = join_table_with_trees_by_one_var(
          partial_result, item_pair.first.first, item_pair.first.second,
          item_pair, k2trees_map, vim);
      partial_results[item_pair.first.second] = partial_result;
    } else if (partial_results.find(item_pair.first.second) !=
               partial_results.end()) {
      // The second variable only is in a single var table
      auto &partial_result = partial_results[item_pair.first.second];
      partial_result = join_table_with_trees_by_one_var(
          partial_result, item_pair.first.second, item_pair.first.first,
          item_pair, k2trees_map, vim);
      partial_results[item_pair.first.first] = partial_result;
    } else {
      // None of the variables is in a single var table
      partial_results[item_pair.first.first] =
          join_two_var_group(item_pair.first.first, item_pair.first.second,
                             item_pair.second, k2trees_map, vim);
      partial_results[item_pair.first.second] =
          partial_results[item_pair.first.first];
    }
  }

  return cross_product_partial_results(partial_results);
}

void process_expr_node(const proto_msg::ExprNode &,
                       std::shared_ptr<ResultTable> &, Cache::cm_t &) {}

std::shared_ptr<ResultTable>
process_left_join_node(const proto_msg::LeftJoinNode &left_join_node,
                       Cache::cm_t &cm, VarIndexManager &vim) {
  // TODO: FIX
  auto left_result = process_join_node(left_join_node.left_node(), cm, vim);
  const auto right_type = left_join_node.right_node().node_case();
  if (right_type == proto_msg::SparqlNode::NodeCase::kBgpNode) {
    const auto &bgp_node = left_join_node.right_node().bgp_node();
    auto k2trees_map = get_k2trees_map_by_predicate_value(bgp_node, cm);
    for (int i = 0; i < bgp_node.triple_size(); i++) {
      const auto &current_triple = bgp_node.triple(i);
      /*
      K2TreeMixed *current_k2tree =
          k2trees_map[current_triple.subject().term_value()];
          */
      auto subject_is_var =
          current_triple.subject().term_type() == proto_msg::TermType::VARIABLE;
      auto object_is_var =
          current_triple.object().term_type() == proto_msg::TermType::VARIABLE;
      if (subject_is_var && object_is_var) {

      } else if (subject_is_var) {

      } else if (object_is_var) {

      } else {
        throw std::runtime_error("found triple without variables: " +
                                 current_triple.subject().term_value() + ", " +
                                 current_triple.predicate().term_value() +
                                 ", " + current_triple.object().term_value());
      }
    }
  } else if (right_type == proto_msg::SparqlNode::NodeCase::kLeftJoinNode) {

  } else {
    throw std::runtime_error(
        "process_left_join_node: unexpected right node type: " +
        std::to_string(right_type));
  }

  return left_result;
}

void remove_extra_vars_from_table(std::shared_ptr<ResultTable> input_table,
                                  const std::vector<std::string> &vars,
                                  VarIndexManager &vim) {

  // std::unordered_set<std::string> vars_set(vars.begin(), vars.end());
  std::unordered_set<unsigned long> vars_set;
  std::unordered_set<unsigned long> columns_to_erase;
  for (auto var : vars) {
    vars_set.insert(vim.var_indexes[var]);
  }

  std::unordered_set<unsigned long> headers_set;
  for (auto var_value : input_table->headers) {
    headers_set.insert(var_value);
  }

  if (vars_set == headers_set)
    return;

  std::vector<unsigned long> next_headers;
  for (size_t h_i = 0; h_i < input_table->headers.size(); h_i++) {
    auto header_value = input_table->headers[h_i];
    if (vars_set.find(header_value) == vars_set.end()) {
      columns_to_erase.insert(h_i);
    } else {
      next_headers.push_back(header_value);
    }
  }

  input_table->headers = std::move(next_headers);

  for (auto &row : input_table->data) {
    std::vector<unsigned long> next_row;
    for (size_t i = 0; i < row.size(); i++) {
      if (columns_to_erase.find(i) == columns_to_erase.end()) {
        next_row.push_back(row[i]);
      }
    }
    row = std::move(next_row);
  }
}

void remove_repeated_rows(std::shared_ptr<ResultTable> input_table) {
  auto left_it = input_table->data.begin();

  for (auto it = std::next(left_it); it != input_table->data.end();) {
    if (*it == *left_it) {
      auto next_it = std::next(it);
      input_table->data.erase(it);
      it = next_it;
    } else {
      left_it = it;
      it++;
    }
  }
}

std::shared_ptr<ResultTable>
process_project_node(const proto_msg::ProjectNode &project_node,
                     Cache::cm_t &cm, VarIndexManager &vim) {
  std::vector<std::string> vars;
  for (int i = 0; i < project_node.vars_size(); i++) {
    vars.push_back(project_node.vars(i));
  }

  for (const auto &var : vars) {
    vim.assign_index_if_not_found(var);
  }

  std::shared_ptr<ResultTable> result_table;

  switch (project_node.sub_op().node_case()) {
  case proto_msg::SparqlNode::NodeCase::kBgpNode:
    result_table = process_bgp_node(project_node.sub_op().bgp_node(), cm, vim);
    break;
  case proto_msg::SparqlNode::NodeCase::kLeftJoinNode:
    result_table =
        process_left_join_node(project_node.sub_op().left_join_node(), cm, vim);
    break;
  default:
    throw std::runtime_error("Unsupported nodetype on process_project_node: " +
                             std::to_string(project_node.sub_op().node_case()));
  }

  remove_extra_vars_from_table(result_table, vars, vim);
  // remove_repeated_rows(result_table);
  return result_table;
}

void process_triple_node(const proto_msg::TripleNode &,
                         std::shared_ptr<ResultTable> &, Cache::cm_t &) {}

std::shared_ptr<ResultTable> process_node(const proto_msg::SparqlNode &node,
                                          Cache::cm_t &cm,
                                          VarIndexManager &vim) {
  if (node.node_case() != proto_msg::SparqlNode::NodeCase::kProjectNode)
    throw std::runtime_error("(process_node) Expected ProjectNode, but got: " +
                             std::to_string(node.node_case()));
  return process_project_node(node.project_node(), cm, vim);
}

QueryResult Cache::run_query(proto_msg::SparqlTree const &query_tree) {
  VarIndexManager vim;
  auto result = process_node(query_tree.root(), cache_manager, vim);
  return QueryResult(result, std::move(vim));
}

std::string Cache::extract_resource(unsigned long index) const {
  return cache_manager->extract_resource(index);
}

PredicatesCacheManager &Cache::get_pcm() { return *cache_manager; }
