#include "BGPProcessor.hpp"

#include <stdexcept>

// very slow #include "BandMapLazy.hpp"
#include "BandMap.hpp"

BGPProcessor::BGPProcessor(const proto_msg::BGPNode &bgp_node,
                           const PredicatesCacheManager &cm,
                           VarIndexManager &vim)
    : bgp_node(bgp_node), cm(cm), vim(vim) {
  set_triples_from_proto();
}

std::shared_ptr<ResultTable> BGPProcessor::execute() {

  fill_table_with_first_triple();

  for (size_t i = 1; i < triples.size(); i++) {
    combine_triple_with_table(triples[i]);
  }

  return current_table;
}

void BGPProcessor::set_triples_from_proto() {
  for (int i = 0; i < bgp_node.triple_size(); i++) {
    const auto &triple = bgp_node.triple()[i];
    triples.push_back(Triple(triple, cm));
  }
}

void BGPProcessor::fill_table_with_first_triple() {
  const auto &triple = triples[0];

  current_table = std::make_shared<ResultTable>();

  if (triple.subject.type == TermType::VAR &&
      triple.object.type == TermType::VAR) {
    add_full_predicate_to_table(triple);
  } else if (triple.subject.type == TermType::VAR) {
    add_row_to_table(triple);
  } else if (triple.object.type == TermType::VAR) {
    add_column_to_table(triple);
  } else {
    throw std::runtime_error("invalid triple");
  }
}

void BGPProcessor::add_full_predicate_to_table(const Triple &triple) {
  K2TreeMixed *tree = nullptr;
  if (triple.predicate.id_value != 0) {
    auto fetched = cm.get_tree_by_predicate_index(triple.predicate.id_value);
    tree = fetched.get_ptr();
  }

  vim.assign_index_if_not_found(triple.subject.value);
  vim.assign_index_if_not_found(triple.object.value);

  const unsigned long subject_var_index = vim.var_indexes[triple.subject.value];
  const unsigned long object_var_index = vim.var_indexes[triple.object.value];
  current_table->headers.push_back(subject_var_index);
  current_table->headers.push_back(object_var_index);

  if (tree)
    tree->scan_points(
        [](unsigned long col, unsigned long row, void *table_ptr) {
          reinterpret_cast<ResultTable *>(table_ptr)->add_row({col, row});
        },
        current_table.get());
}

void BGPProcessor::add_row_to_table(const Triple &triple) {
  K2TreeMixed *tree = nullptr;
  if (triple.predicate.id_value != 0) {
    auto fetch_result =
        cm.get_tree_by_predicate_index(triple.predicate.id_value);
    tree = fetch_result.get_ptr();
  }

  vim.assign_index_if_not_found(triple.subject.value);
  const unsigned long subject_var_index = vim.var_indexes[triple.subject.value];
  current_table->headers.push_back(subject_var_index);

  if (tree && triple.object.id_value != 0)
    tree->traverse_row(
        triple.object.id_value,
        [](unsigned long col, unsigned long, void *table_ptr) {
          reinterpret_cast<ResultTable *>(table_ptr)->add_row({col});
        },
        current_table.get());
}

void BGPProcessor::add_column_to_table(const Triple &triple) {
  K2TreeMixed *tree = nullptr;
  if (triple.predicate.id_value != 0) {
    auto fetch_result =
        cm.get_tree_by_predicate_index(triple.predicate.id_value);
    tree = fetch_result.get_ptr();
  }

  vim.assign_index_if_not_found(triple.object.value);
  const unsigned long object_var_index = vim.var_indexes[triple.object.value];
  current_table->headers.push_back(object_var_index);

  if (tree && triple.subject.id_value != 0)
    tree->traverse_column(
        triple.subject.id_value,
        [](unsigned long, unsigned long row, void *table_ptr) {
          reinterpret_cast<ResultTable *>(table_ptr)->add_row({row});
        },
        current_table.get());
}

void BGPProcessor::combine_triple_with_table(const Triple &triple) {
  if (triple.subject.type == TermType::VAR &&
      triple.object.type == TermType::VAR) {
    combine_full_predicate_to_table(triple);
  } else if (triple.subject.type == TermType::VAR) {
    combine_row_to_table(triple);
  } else if (triple.object.type == TermType::VAR) {
    combine_column_to_table(triple);
  } else {
    throw std::runtime_error("invalid triple");
  }
}

void BGPProcessor::combine_full_predicate_to_table(const Triple &triple) {
  vim.assign_index_if_not_found(triple.subject.value);
  vim.assign_index_if_not_found(triple.object.value);

  // const auto &headers = current_table->headers;

  const unsigned long subject_var_index = vim.var_indexes[triple.subject.value];
  const unsigned long object_var_index = vim.var_indexes[triple.object.value];
  const int subject_pos_in_headers =
      find_var_position_in_headers(subject_var_index);
  const int object_pos_in_headers =
      find_var_position_in_headers(object_var_index);

  if (subject_pos_in_headers == -1 && object_pos_in_headers == -1) {
    cross_product_table_with_triple(triple);
  } else if (subject_pos_in_headers != -1 && object_pos_in_headers != -1) {
    intersect_table_with_predicate(triple, subject_pos_in_headers,
                                   object_pos_in_headers);
  } else if (subject_pos_in_headers != -1) {
    left_join_table_with_triple_subject(triple, subject_pos_in_headers);
  } else {
    left_join_table_with_triple_object(triple, object_pos_in_headers);
  }
}

int BGPProcessor::find_var_position_in_headers(unsigned long var_index) {
  auto &headers = current_table->headers;

  for (int i = 0; i < static_cast<int>(headers.size()); i++) {
    if (headers[i] == var_index)
      return i;
  }

  return -1;
}

void BGPProcessor::cross_product_table_with_triple(const Triple &triple) {

  std::vector<std::pair<unsigned long, unsigned long>> predicate_points;
  ;

  if (triple.predicate.id_value != 0) {
    const auto fetch_result =
        cm.get_tree_by_predicate_index(triple.predicate.id_value);
    const auto &tree = fetch_result.get();

    predicate_points = tree.scan_points_into_vector();
  }

  current_table->headers.push_back(vim.var_indexes[triple.subject.value]);
  current_table->headers.push_back(vim.var_indexes[triple.object.value]);
  auto &table_data = current_table->get_data();

  for (auto row_it = table_data.begin(); row_it != table_data.end();) {
    auto &row = *row_it;

    auto next_it = std::next(row_it);
    for (const auto &point : predicate_points) {
      auto row_copy = row;
      row_copy.push_back(point.first);
      row_copy.push_back(point.second);
      table_data.insert(next_it, std::move(row_copy));
    }

    table_data.erase(row_it);
    row_it = next_it;
  }
}

void BGPProcessor::left_join_table_with_triple_subject(
    const Triple &triple, unsigned long subject_var_index) {

  BandMap band_map;
  if (triple.predicate.id_value != 0) {
    const auto fetch_result =
        cm.get_tree_by_predicate_index(triple.predicate.id_value);
    const auto &tree = fetch_result.get();

    band_map = BandMap(tree, BandMap::BY_COL);
  }

  auto &table_data = current_table->get_data();
  auto &headers = current_table->headers;

  headers.push_back(vim.var_indexes[triple.object.value]);

  for (auto row_it = table_data.begin(); row_it != table_data.end();) {
    auto &row = *row_it;
    auto table_value = row[subject_var_index];
    auto next_it = std::next(row_it);

    for (unsigned long object_coord : band_map[table_value]) {
      auto row_copy = row;
      row_copy.push_back(object_coord);
      table_data.insert(next_it, std::move(row_copy));
    }

    table_data.erase(row_it);
    row_it = next_it;
  }
}

void BGPProcessor::left_join_table_with_triple_object(
    const Triple &triple, unsigned long object_var_index) {
  BandMap band_map;
  if (triple.predicate.id_value != 0) {

    const auto fetch_result =
        cm.get_tree_by_predicate_index(triple.predicate.id_value);
    const auto &tree = fetch_result.get();

    band_map = BandMap(tree, BandMap::BY_ROW);
  }

  auto &table_data = current_table->get_data();
  auto &headers = current_table->headers;

  headers.push_back(vim.var_indexes[triple.subject.value]);

  for (auto row_it = table_data.begin(); row_it != table_data.end();) {
    auto &row = *row_it;
    auto table_value = row[object_var_index];
    auto next_it = std::next(row_it);

    for (unsigned long subject_coord : band_map[table_value]) {
      auto row_copy = row;
      row_copy.push_back(subject_coord);
      table_data.insert(next_it, std::move(row_copy));
    }
    table_data.erase(row_it);
    row_it = next_it;
  }
}

void BGPProcessor::intersect_table_with_predicate(
    const Triple &triple, unsigned long subject_var_index,
    unsigned long object_var_index) {

  K2TreeMixed *tree = nullptr;
  if (triple.predicate.id_value != 0) {
    auto fetch_result =
        cm.get_tree_by_predicate_index(triple.predicate.id_value);
    tree = fetch_result.get_ptr();
  }

  auto &table_data = current_table->get_data();

  for (auto row_it = table_data.begin(); row_it != table_data.end();) {
    auto next_it = std::next(row_it);
    auto &row = *row_it;

    auto subject_value = row[subject_var_index];
    auto object_value = row[object_var_index];

    if (!tree || !tree->has(subject_value, object_value)) {
      table_data.erase(row_it);
    }
    row_it = next_it;
  }
}

void BGPProcessor::combine_column_to_table(const Triple &triple) {
  // object is variable
  vim.assign_index_if_not_found(triple.object.value);

  const unsigned long object_var_index = vim.var_indexes[triple.object.value];
  const int object_pos_in_headers =
      find_var_position_in_headers(object_var_index);

  if (object_pos_in_headers == -1) {
    cross_product_table_with_band(triple, COL_BAND);
  } else {
    intersect_table_with_band(object_pos_in_headers, triple, COL_BAND);
  }
}

void BGPProcessor::combine_row_to_table(const Triple &triple) {
  // subject is variable
  vim.assign_index_if_not_found(triple.subject.value);

  const unsigned long subject_var_index = vim.var_indexes[triple.subject.value];
  const int subject_pos_in_headers =
      find_var_position_in_headers(subject_var_index);

  if (subject_pos_in_headers == -1) {
    cross_product_table_with_band(triple, ROW_BAND);
  } else {
    intersect_table_with_band(subject_pos_in_headers, triple, ROW_BAND);
  }
}

void BGPProcessor::cross_product_table_with_band(const Triple &triple,
                                                 BAND_TYPE band_type) {
  K2TreeMixed *tree = nullptr;
  if (triple.predicate.id_value != 0) {
    auto fetch_result =
        cm.get_tree_by_predicate_index(triple.predicate.id_value);
    if (!fetch_result.exists())
      throw std::runtime_error(
          "Predicate not found: " + triple.predicate.value +
          ", with id: " + std::to_string(triple.predicate.id_value));
    tree = fetch_result.get_ptr();
  }

  auto &table_data = current_table->get_data();
  auto &headers = current_table->headers;

  std::vector<unsigned long> band;
  if (band_type == COL_BAND) {
    if (tree)
      band = tree->get_column(triple.subject.id_value);
    headers.push_back(vim.var_indexes[triple.object.value]);
  } else {
    if (tree)
      band = tree->get_row(triple.object.id_value);
    headers.push_back(vim.var_indexes[triple.subject.value]);
  }

  for (auto row_it = table_data.begin(); row_it != table_data.end();) {
    auto next_it = std::next(row_it);
    auto &row = *row_it;
    for (auto band_value : band) {
      auto row_copy = row;
      row_copy.push_back(band_value);
      table_data.insert(next_it, std::move(row_copy));
    }
    table_data.erase(row_it);
    row_it = next_it;
  }
}

void BGPProcessor::intersect_table_with_band(unsigned long pos_in_headers,
                                             const Triple &triple,
                                             BAND_TYPE band_type) {
  K2TreeMixed *tree = nullptr;
  if (triple.predicate.id_value) {
    auto fetch_result =
        cm.get_tree_by_predicate_index(triple.predicate.id_value);
    tree = fetch_result.get_ptr();
  }

  auto &table_data = current_table->get_data();

  std::vector<unsigned long> band;
  if (band_type == COL_BAND) {
    if (tree && triple.subject.id_value != 0)
      band = tree->get_column(triple.subject.id_value);
  } else {
    if (tree && triple.object.id_value != 0)
      band = tree->get_row(triple.object.id_value);
  }

  std::unordered_set<unsigned long> band_set(band.begin(), band.end());
  band.clear();

  for (auto row_it = table_data.begin(); row_it != table_data.end();) {
    auto next_it = std::next(row_it);
    auto &row = *row_it;

    auto table_value = row[pos_in_headers];

    if (band_set.find(table_value) == band_set.end()) {
      table_data.erase(row_it);
    }

    row_it = next_it;
  }
}
