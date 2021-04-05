#include "BGPProcessor.hpp"

#include "ChainedInput.hpp"
#include "MergeJoin.hpp"
#include "TwoVarGroup.hpp"
#include "mutable_pair.hpp"

#include <sstream>

BGPProcessor::BGPProcessor(const proto_msg::BGPNode &bgp_node,
                           const PredicatesCacheManager &cm,
                           VarIndexManager &vim)
    : bgp_node(bgp_node), cm(cm), vim(vim) {}

std::shared_ptr<ResultTable> BGPProcessor::execute() {
  auto start = std::chrono::high_resolution_clock::now();

  auto groups = group_triple_nodes();
  auto &one_var_groups = groups.one_var_groups;
  auto &two_var_groups = groups.two_var_groups;

  std::unordered_map<std::string, std::shared_ptr<ResultTable>> partial_results;
  for (const auto &item : one_var_groups) {
    vim.assign_index_if_not_found(item.first);
    partial_results[item.first] =
        join_single_var_group(item.first, item.second);
  }

  std::vector<TwoVarGroup> two_var_groups_ordered;

  for (const auto &item_pair : two_var_groups) {
    TwoVarGroup group;
    group.keys = item_pair.first;
    group.triples = item_pair.second;
    two_var_groups_ordered.push_back(std::move(group));
  }
  std::sort(two_var_groups_ordered.begin(), two_var_groups_ordered.end(),
            [this](const TwoVarGroup &lhs, const TwoVarGroup &rhs) {
              auto &lhs_predicate_str = lhs.triples[0].predicate.value;
              auto &rhs_predicate_str = rhs.triples[0].predicate.value;
              auto &lhs_tree = cm.get_tree_by_predicate_name(lhs_predicate_str);
              auto &rhs_tree = cm.get_tree_by_predicate_name(rhs_predicate_str);
              return lhs_tree.size() < rhs_tree.size();
            });

  for (auto &tv_group : two_var_groups_ordered) {

    auto &var_pair = tv_group.keys;
    auto &triples = tv_group.triples;
    vim.assign_index_if_not_found(var_pair.first_var);
    vim.assign_index_if_not_found(var_pair.second_var);

    if (partial_results.find(var_pair.first_var) != partial_results.end() &&
        partial_results.find(var_pair.second_var) != partial_results.end()) {
      // Both variables are in single var table
      auto &partial_result_one = partial_results[var_pair.first_var];
      auto &partial_result_two = partial_results[var_pair.second_var];
      std::shared_ptr<ResultTable> result;
      if (&partial_result_one != &partial_result_two) {

        result = join_two_tables_with_trees(
            partial_result_one, partial_result_two, var_pair, triples);
      } else {
        result = join_table_with_trees_by_two_var(partial_result_one, var_pair,
                                                  triples);
      }

      partial_results[var_pair.first_var] = result;
      partial_results[var_pair.second_var] = result;
    } else if (partial_results.find(var_pair.first_var) !=
               partial_results.end()) {
      // The first variable only is in a single var table
      auto &partial_result = partial_results[var_pair.first_var];
      std::cout << "join_table_with_trees_by_one_var bef size: "
                << partial_result->rows_size() << std::endl;
      partial_result = join_table_with_trees_by_one_var(
          partial_result, var_pair.first_var, var_pair.second_var, triples);
      partial_results[var_pair.second_var] = partial_result;
      partial_results[var_pair.first_var] = partial_result;
      std::cout << "join_table_with_trees_by_one_var after size: "
                << partial_result->rows_size() << std::endl;
    } else if (partial_results.find(var_pair.second_var) !=
               partial_results.end()) {
      // The second variable only is in a single var table
      auto &partial_result = partial_results[var_pair.second_var];
      std::cout << "join_table_with_trees_by_one_var bef size: "
                << partial_result->rows_size() << std::endl;
      partial_result = join_table_with_trees_by_one_var(
          partial_result, var_pair.second_var, var_pair.first_var, triples);
      partial_results[var_pair.first_var] = partial_result;
      partial_results[var_pair.second_var] = partial_result;
      std::cout << "join_table_with_trees_by_one_var after size: "
                << partial_result->rows_size() << std::endl;
    } else {
      // None of the variables is in a single var table
      partial_results[var_pair.first_var] =
          join_two_var_group(var_pair.first_var, var_pair.second_var, triples);
      partial_results[var_pair.second_var] =
          partial_results[var_pair.first_var];
    }
  }

  auto result = cross_product_partial_results(partial_results);
  auto end = std::chrono::high_resolution_clock::now();
  auto duration =
      std::chrono::duration_cast<std::chrono::milliseconds>(end - start);
  std::cout << "process_bgp_node took " << duration.count() << " ms"
            << std::endl;

  return result;
}

GroupedTriples BGPProcessor::group_triple_nodes() {

  std::unordered_map<std::string, std::vector<Triple>> one_var_groups;
  std::unordered_map<KeyPairStr, std::vector<Triple>, hash_pair> two_var_groups;

  for (int i = 0; i < bgp_node.triple_size(); i++) {
    const auto &triple = bgp_node.triple().at(i);

    if (triple.subject().term_type() == proto_msg::TermType::VARIABLE &&
        triple.object().term_type() == proto_msg::TermType::VARIABLE) {
      KeyPairStr keypair;
      if (triple.subject().term_value() < triple.object().term_value()) {
        keypair = KeyPairStr(triple.subject().term_value(),
                             triple.object().term_value());
      } else {
        keypair = KeyPairStr(triple.object().term_value(),
                             triple.subject().term_value());
      }

      two_var_groups[keypair].push_back(Triple(triple, cm));

    } else if (triple.subject().term_type() == proto_msg::TermType::VARIABLE) {
      one_var_groups[triple.subject().term_value()].push_back(
          Triple(triple, cm));
    } else if (triple.object().term_type() == proto_msg::TermType::VARIABLE) {
      one_var_groups[triple.object().term_value()].push_back(
          Triple(triple, cm));
    } else {
      std::stringstream ss;
      ss << "Found a triple without variable: ";
      ss << "(" << triple.subject().term_value() << ", "
         << triple.predicate().term_value() << ", "
         << triple.object().term_value() << ")";

      throw std::runtime_error(ss.str());
    }
  }

  return GroupedTriples(std::move(one_var_groups), std::move(two_var_groups));
}

std::shared_ptr<ResultTable>
BGPProcessor::join_single_var_group(const std::string &var_name,
                                    const std::vector<Triple> &triples) {

  std::vector<const K2TreeMixed *> join_trees;
  std::vector<struct sip_ipoint> join_coordinates;
  for (const auto &triple : triples) {
    const auto *tree =
        &cm.get_tree_by_predicate_index(triple.predicate.id_value);
    join_trees.push_back(tree);
    struct sip_ipoint current_sip_ipoint;
    // subjects are columns and objects are rows
    if (triple.subject.type == TermType::VAR) {
      current_sip_ipoint.coord = get_index_from_term(triple.object);
      current_sip_ipoint.coord_type = ROW_COORD;
    } else {
      current_sip_ipoint.coord = get_index_from_term(triple.subject);
      current_sip_ipoint.coord_type = COLUMN_COORD;
    }
    join_coordinates.push_back(current_sip_ipoint);
  }

  std::vector<unsigned long> join_result =
      K2TreeMixed::sip_join_k2trees(join_trees, join_coordinates);

  return std::make_shared<ResultTable>(vim.var_indexes[var_name],
                                       std::move(join_result));
}

std::shared_ptr<ResultTable> BGPProcessor::join_two_tables_with_trees(
    std::shared_ptr<ResultTable> &table_one,
    std::shared_ptr<ResultTable> &table_two, KeyPairStr &keypair,
    std::vector<Triple> &triples) {

  auto start = std::chrono::high_resolution_clock::now();
  std::shared_ptr<ResultTable> smaller, bigger;
  const std::string *smaller_var, *bigger_var;
  if (table_one->rows_size() < table_two->rows_size()) {
    smaller = table_one;
    bigger = table_two;

    smaller_var = &keypair.first_var;
    bigger_var = &keypair.second_var;
  } else {
    smaller = table_two;
    bigger = table_one;

    smaller_var = &keypair.second_var;
    bigger_var = &keypair.first_var;
  }

  unsigned long smaller_index = vim.var_indexes[*smaller_var];
  unsigned long smaller_real_index = smaller->get_actual_index(smaller_index);

  unsigned long bigger_index = vim.var_indexes[*bigger_var];
  unsigned long bigger_real_index = bigger->get_actual_index(bigger_index);

  for (const auto &row : smaller->data) {

    unsigned long current_cord_smaller = row.at(smaller_real_index);
    for (const Triple &triple : triples) {
      std::vector<unsigned long> retrieved;
      if (triple.subject.value == *smaller_var) {
        retrieved = cm.get_tree_by_predicate_index(triple.predicate.id_value)
                        .get_row(current_cord_smaller);
      } else {
        // is object
        retrieved = cm.get_tree_by_predicate_index(triple.predicate.id_value)
                        .get_column(current_cord_smaller);
      }
      bigger->left_join_with_vector(bigger_real_index, retrieved, false);
    }
  }

  std::unordered_map<std::string, std::shared_ptr<ResultTable>> partial_results;

  partial_results[*smaller_var] = smaller;
  partial_results[*bigger_var] = bigger;

  auto result = cross_product_partial_results(partial_results);
  auto stop = std::chrono::high_resolution_clock::now();
  auto duration =
      std::chrono::duration_cast<std::chrono::milliseconds>(stop - start);

  std::cout << "join_two_tables_with_trees took " << duration.count() << " ms"
            << std::endl;

  return result;
}

std::shared_ptr<ResultTable> BGPProcessor::join_table_with_trees_by_two_var(
    std::shared_ptr<ResultTable> &table, KeyPairStr &keypair,
    std::vector<Triple> &triples) {
  auto start = std::chrono::high_resolution_clock::now();

  const std::string &left_string = keypair.first_var;
  const std::string &right_string = keypair.second_var;

  unsigned long left_index = vim.var_indexes[left_string];
  unsigned long left_real_index = table->get_actual_index(left_index);
  unsigned long right_index = vim.var_indexes[right_string];
  unsigned long right_real_index = table->get_actual_index(right_index);

  for (auto it = table->data.begin(); it != table->data.end(); it++) {
    auto &row = *it;
    unsigned long left_value = row[left_real_index];
    unsigned long right_value = row[right_real_index];
    for (const auto &triple : triples) {
      const auto &k2tree =
          cm.get_tree_by_predicate_index(triple.predicate.id_value);
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

  auto stop = std::chrono::high_resolution_clock::now();
  auto duration =
      std::chrono::duration_cast<std::chrono::milliseconds>(stop - start);

  std::cout << "join_table_with_trees_by_two_var took " << duration.count()
            << " ms" << std::endl;

  return table;
}

std::shared_ptr<ResultTable> BGPProcessor::join_table_with_trees_by_one_var(
    std::shared_ptr<ResultTable> &table, const std::string &table_var,
    const std::string &other_var, std::vector<Triple> &triples) {

  auto start = std::chrono::high_resolution_clock::now();

  unsigned long table_var_index = vim.var_indexes[table_var];
  unsigned long real_table_var_index = table->get_actual_index(table_var_index);

  vim.assign_index_if_not_found(other_var);
  unsigned long other_var_index = vim.var_indexes[other_var];

  table->headers.push_back(other_var_index);

  // const auto &var_pair = two_var_group_item.first;

  std::vector<unsigned long> triples_subject_ids;

  // to avoid comparing strings too much
  for (const auto &triple : triples) {
    triples_subject_ids.push_back(vim.var_indexes[triple.subject.value]);
  }

  for (auto it = table->data.begin(); it != table->data.end();) {
    unsigned long current_table_value = it->at(real_table_var_index);

    int triple_index = 0;
    std::unique_ptr<std::vector<unsigned long>> resulting_band;
    for (const auto &triple : triples) {
      auto &k2tree = cm.get_tree_by_predicate_index(triple.predicate.id_value);

      std::vector<unsigned long> current_band;
      if (triples_subject_ids[triple_index] == table_var_index) {
        // std::cout << "matches subject, traversing column " <<
        // current_table_value << std::endl;
        k2tree.traverse_column(
            current_table_value,
            [](unsigned long, unsigned long row, void *report_state) {
              reinterpret_cast<std::vector<unsigned long> *>(report_state)
                  ->push_back(row);
            },
            &current_band);

      } else {
        // std::cout << "doesn't match subject, traversing row " <<
        // current_table_value << std::endl;

        k2tree.traverse_row(
            current_table_value,
            [](unsigned long col, unsigned long, void *report_state) {
              reinterpret_cast<std::vector<unsigned long> *>(report_state)
                  ->push_back(col);
            },
            &current_band);
      }

      if (triple_index == 0) {
        // std::cout << "current_band  size: " << current_band.size() <<
        // std::endl;
        resulting_band = std::make_unique<std::vector<unsigned long>>(
            std::move(current_band));
      } else {
        // std::cout << "current_band  size: " << current_band.size() <<
        // std::endl;
        resulting_band = std::make_unique<std::vector<unsigned long>>(
            MergeJoin::merge_vectors(*resulting_band, current_band));
        // std::cout << "merge vectors result size: " << resulting_band.size()
        // << std::endl;
      }

      triple_index++;
    }

    auto next_it = std::next(it);
    if (resulting_band->size() == 0) {
      table->data.erase(it);
    } else {
      for (size_t curr_val_i = 0; curr_val_i < resulting_band->size();
           curr_val_i++) {
        auto value = resulting_band->at(curr_val_i);
        auto &current_row = *it;
        auto row_copy = current_row;
        current_row.push_back(value);
        if (curr_val_i < resulting_band->size() - 1)
          it = table->data.insert(next_it, std::move(row_copy));
      }
    }

    it = next_it;
  }

  auto stop = std::chrono::high_resolution_clock::now();
  auto duration =
      std::chrono::duration_cast<std::chrono::milliseconds>(stop - start);

  std::cout << "join_table_with_trees_by_one_var took " << duration.count()
            << " ms" << std::endl;

  return table;
}

std::shared_ptr<ResultTable> BGPProcessor::join_table_with_trees_by_one_var_sip(
    std::shared_ptr<ResultTable> &table, const std::string &table_var,
    const std::string &other_var, std::vector<Triple> &triples) {

  auto start = std::chrono::high_resolution_clock::now();

  unsigned long table_var_index = vim.var_indexes[table_var];
  unsigned long real_table_var_index = table->get_actual_index(table_var_index);

  vim.assign_index_if_not_found(other_var);
  unsigned long other_var_index = vim.var_indexes[other_var];

  table->headers.push_back(other_var_index);

  // const auto &var_pair = two_var_group_item.first;

  std::vector<unsigned long> triples_subject_ids;

  // to avoid comparing strings too much
  for (const auto &triple : triples) {
    triples_subject_ids.push_back(vim.var_indexes[triple.subject.value]);
  }

  for (auto it = table->data.begin(); it != table->data.end();) {
    auto &row = *it;
    unsigned long current_table_value = row[real_table_var_index];
    std::vector<const K2TreeMixed *> join_trees;
    std::vector<struct sip_ipoint> join_coordinates;
    int triple_index = 0;
    for (const auto &triple : triples) {
      join_trees.push_back(&cm.get_tree_by_predicate_index(
          triple.predicate.id_value)); // TODO:use numeric keys instead
      struct sip_ipoint join_point;
      join_point.coord = current_table_value;
      if (triples_subject_ids[triple_index] == table_var_index) {
        join_point.coord_type = coord_t::COLUMN_COORD;
      } else {
        join_point.coord_type = coord_t::ROW_COORD;
      }

      join_coordinates.push_back(join_point);
      triple_index++;
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

  auto stop = std::chrono::high_resolution_clock::now();
  auto duration =
      std::chrono::duration_cast<std::chrono::milliseconds>(stop - start);

  std::cout << "join_table_with_trees_by_one_var_sip took " << duration.count()
            << " ms" << std::endl;

  return table;
}

std::shared_ptr<ResultTable>
BGPProcessor::join_two_var_group(const std::string &var_one,
                                 const std::string &var_two,
                                 const std::vector<Triple> &triples) {

  auto start = std::chrono::high_resolution_clock::now();

  if (triples.empty())
    throw std::runtime_error("join_two_var_group: no triples");

  std::vector<Triple> triples_rearranged(triples);

  std::sort(
      triples_rearranged.begin(), triples_rearranged.end(),
      [this](const Triple &lhs, const Triple &rhs) {
        return cm.get_tree_by_predicate_index(lhs.predicate.id_value).size() <
               cm.get_tree_by_predicate_index(rhs.predicate.id_value).size();
      });

  const auto &first_triple = triples_rearranged.at(0);
  const K2TreeMixed &first_k2tree =
      cm.get_tree_by_predicate_index(first_triple.predicate.id_value);

  std::cout << "join_two_var_group first_triple predicate: "
            << first_triple.predicate.value << std::endl;

  auto result = std::make_shared<ResultTable>(std::vector<unsigned long>(
      {vim.var_indexes[var_one], vim.var_indexes[var_two]}));

  std::vector<coord_t> coord_types;

  for (auto &triple : triples_rearranged) {
    coord_t ctype;
    if (triple.subject.value == var_one) {
      ctype = COLUMN_COORD;
    } else {
      ctype = ROW_COORD;
    }
    coord_types.push_back(ctype);
  }

  ChainedInput chained_input(*result, triples_rearranged, cm, coord_types);

  first_k2tree.scan_points(
      [](unsigned long col, unsigned long row, void *report_state) {
        auto &chained_input = *reinterpret_cast<ChainedInput *>(report_state);
        auto &result_table = chained_input.result_table;
        const auto &triples = chained_input.triples;
        const auto &cm = chained_input.cm;
        const auto &coord_types = chained_input.coord_types;
        for (size_t i = 1; i < triples.size(); i++) {
          auto &triple = triples[i];
          auto &k2tree =
              cm.get_tree_by_predicate_index(triple.predicate.id_value);
          if (coord_types[i] == COLUMN_COORD) {
            if (!k2tree.has(col, row))
              return;
          } else {
            if (!k2tree.has(row, col))
              return;
          }
        }

        if (coord_types[0] == COLUMN_COORD)
          result_table.data.push_back(std::vector<unsigned long>({col, row}));
        else
          result_table.data.push_back(std::vector<unsigned long>({row, col}));
      },
      &chained_input);

  auto stop = std::chrono::high_resolution_clock::now();
  auto duration =
      std::chrono::duration_cast<std::chrono::milliseconds>(stop - start);

  std::cout << "join_two_var_group took " << duration.count() << " ms"
            << std::endl;

  return result;
}

std::shared_ptr<ResultTable> BGPProcessor::cross_product_partial_results(
    std::unordered_map<std::string, std::shared_ptr<ResultTable>>
        &partial_results) {

  auto start = std::chrono::high_resolution_clock::now();

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

  std::cout << "plain partial results size : " << plain_partial_results.size()
            << std::endl;

  std::set<ResultTable *> done;

  done.insert(left_table.get());

  for (const auto &pair : plain_partial_results) {
    const auto &var_name = pair.first;
    auto table = pair.second;
    if (done.find(table.get()) != done.end()) {
      continue;
    }

    if (left_table.get() != table.get()) {
      std::cout << "merging... size left: " << left_table->rows_size()
                << ", size right: " << table->rows_size() << std::endl;
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
    }

    partial_results[var_name] = left_table;
    done.insert(table.get());
  }

  auto end = std::chrono::high_resolution_clock::now();
  auto duration =
      std::chrono::duration_cast<std::chrono::milliseconds>(end - start);
  std::cout << "cross_product_partial_results took " << duration.count()
            << " ms" << std::endl;

  return left_table;
}

static bool less_than_uvtl_mpair(const std::vector<unsigned long> &vec,
                                 mutable_pair pair) {
  return (vec[0] != pair.first && vec[0] < pair.first) ||
         (vec[0] == pair.first && vec[1] < pair.second);
}

std::shared_ptr<ResultTable> BGPProcessor::join_two_var_group_merge(
    const std::string &var_one, const std::string &var_two,
    const std::vector<Triple> &triples,
    std::unordered_map<std::string, K2TreeMixed *> &k2trees_map) {
  auto start = std::chrono::high_resolution_clock::now();

  if (triples.empty())
    throw std::runtime_error("join_two_var_group: no triples");

  std::vector<Triple> triples_rearranged(triples);
  std::sort(triples_rearranged.begin(), triples_rearranged.end(),
            [&k2trees_map](const Triple &lhs, const Triple &rhs) {
              return k2trees_map[lhs.predicate.value]->size() <
                     k2trees_map[rhs.predicate.value]->size();
            });

  const auto &first_triple = triples_rearranged.at(0);
  K2TreeMixed &first_k2tree = *k2trees_map[first_triple.predicate.value];

  auto result = std::make_shared<ResultTable>(std::vector<unsigned long>(
      {vim.var_indexes[var_one], vim.var_indexes[var_two]}));

  if (first_triple.subject.value == var_one) {
    first_k2tree.scan_points(
        [](unsigned long col, unsigned long row, void *report_state) {
          auto &table = *reinterpret_cast<ResultTable *>(report_state);
          table.add_row({col, row});
        },
        result.get());
  } else {
    first_k2tree.scan_points(
        [](unsigned long col, unsigned long row, void *report_state) {
          auto &table = *reinterpret_cast<ResultTable *>(report_state);
          table.add_row({row, col});
        },
        result.get());
  }

  result->sort_rows();

  for (size_t triple_i = 1; triple_i < triples_rearranged.size(); triple_i++) {
    const auto &triple = triples_rearranged[triple_i];
    std::vector<mutable_pair> current_table;
    K2TreeMixed &current_k2tree = *k2trees_map[triple.predicate.value];

    if (triple.subject.value == var_one) {
      current_k2tree.scan_points(
          [](unsigned long col, unsigned long row, void *report_state) {
            auto &current_table =
                *reinterpret_cast<std::vector<mutable_pair> *>(report_state);
            current_table.push_back({col, row});
          },
          &current_table);
    } else {
      current_k2tree.scan_points(
          [](unsigned long col, unsigned long row, void *report_state) {
            auto &current_table =
                *reinterpret_cast<std::vector<mutable_pair> *>(report_state);
            current_table.push_back({row, col});
          },
          &current_table);
    }
    std::sort(current_table.begin(), current_table.end(),
              [](mutable_pair lhs, mutable_pair rhs) {
                return (lhs.first != rhs.first && lhs.first < rhs.first) ||
                       (lhs.first == rhs.first && lhs.second < rhs.second);
              });

    auto result_it = result->data.begin();
    size_t current_i = 0;
    while (result_it != result->data.end() &&
           current_i < current_table.size()) {
      mutable_pair current_pair = current_table[current_i];
      auto &current_result_row = *result_it;
      if (current_result_row[0] == current_pair.first &&
          current_result_row[1] == current_pair.second) {
        result_it++;
        current_i++;
      } else if (less_than_uvtl_mpair(current_result_row, current_pair)) {
        auto del_it = result_it;
        result_it++;
        result->data.erase(del_it);
      } else {
        current_i++;
      }
    }

    while (result_it != result->data.end()) {
      auto del_it = result_it;
      result_it++;
      result->data.erase(del_it);
    }
  }

  auto stop = std::chrono::high_resolution_clock::now();
  auto duration =
      std::chrono::duration_cast<std::chrono::milliseconds>(stop - start);

  std::cout << "join_two_var_group_merge took " << duration.count() << " ms"
            << std::endl;

  return result;
}

std::shared_ptr<ResultTable>
BGPProcessor::join_two_var_group_sip(const std::string &var_one,
                                     const std::string &var_two,
                                     const std::vector<Triple> &triples) {

  auto start = std::chrono::high_resolution_clock::now();

  if (triples.empty())
    throw std::runtime_error("join_two_var_group: no triples");

  const auto &first_triple = triples.at(0);

  const auto &first_k2tree =
      cm.get_tree_by_predicate_index(first_triple.predicate.id_value);

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

  std::vector<const K2TreeMixed *> join_trees;

  for (const auto &triple : triples) {
    const auto *k2tree_ptr =
        &cm.get_tree_by_predicate_index(triple.predicate.id_value);
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

  auto stop = std::chrono::high_resolution_clock::now();
  auto duration =
      std::chrono::duration_cast<std::chrono::milliseconds>(stop - start);

  std::cout << "join_two_var_group_sip took " << duration.count() << " ms"
            << std::endl;

  return table;
}

unsigned long BGPProcessor::get_index_from_term(const Term &term) {
  unsigned long result;
  switch (term.type) {
  case TermType::IRI:
    result = cm.get_iri_index(term.value);
    break;
  case TermType::LITERAL:
    result = cm.get_literal_index(term.value);
    break;
  case TermType::BLANK:
    result = cm.get_blank_index(term.value);
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
