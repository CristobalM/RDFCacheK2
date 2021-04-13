
#include "UnionProcessor.hpp"

#include <algorithm>
#include <stdexcept>

UnionProcessor::UnionProcessor() : current_table(nullptr) {}

std::shared_ptr<ResultTable> UnionProcessor::get_result() {
  return current_table;
}

static std::set<unsigned long>
intersect_ul_sets(std::set<unsigned long> &first,
                  std::set<unsigned long> &second) {
  std::set<unsigned long> result;
  std::set_intersection(first.begin(), first.end(), second.begin(),
                        second.end(), std::inserter(result, result.begin()));
  return result;
}

/*
static void debug_print_table(const ResultTable &table) {
  std::cout << "table...\n";
  for (const auto &row : table.data) {
    for (auto val : row) {
      std::cout << val << ",";
    }
    std::cout << "\n";
  }
}
*/

std::vector<unsigned long> UnionProcessor::as_current_table(
    const std::vector<unsigned long> &row,
    const std::vector<unsigned long> &extra_headers_positions,
    const std::vector<unsigned long> &current_table_same_headers_positions,
    const std::vector<unsigned long> &table_same_headers_positions

) const {
  std::vector<unsigned long> result(current_table->headers.size(), 0);

  for (size_t i = 0; i < result.size() - extra_headers_positions.size(); i++) {
    result[current_table_same_headers_positions[i]] =
        row[table_same_headers_positions[i]];
  }

  for (size_t i = result.size() - extra_headers_positions.size(), j = 0;
       i < result.size(); i++, j++) {
    result[i] = row[extra_headers_positions[j]];
  }

  return result;
}

std::vector<unsigned long> UnionProcessor::find_extra_headers_positions(
    const ResultTable &table,
    const std::set<unsigned long> &extra_headers) const {
  std::vector<unsigned long> result;

  // The order should be the same as in the set, table.headers should always be
  // small
  for (auto it = extra_headers.begin(); it != extra_headers.end(); it++) {
    for (size_t i = 0; i < table.headers.size(); i++) {
      if (*it == table.headers[i]) {
        result.push_back(i);
      }
    }
  }

  return result;
}

std::pair<std::vector<unsigned long>, std::vector<unsigned long>>
UnionProcessor::find_same_headers_positions(
    const ResultTable &table,
    const std::set<unsigned long> &extra_headers) const {
  std::vector<unsigned long> current_table_same_headers_positions;
  std::vector<unsigned long> table_same_headers_positions;

  std::set<unsigned long> all_new_table_headers(table.headers.begin(),
                                                table.headers.end());
  std::set<unsigned long> same_headers;
  std::set_difference(all_new_table_headers.begin(),
                      all_new_table_headers.end(), extra_headers.begin(),
                      extra_headers.end(),
                      std::inserter(same_headers, same_headers.begin()));

  for (size_t i = 0; i < current_table->headers.size() - extra_headers.size();
       i++) {
    if (same_headers.find(current_table->headers[i]) != same_headers.end()) {
      current_table_same_headers_positions.push_back(i);
    }
  }

  for (size_t i = 0; i < table.headers.size(); i++) {
    if (same_headers.find(table.headers[i]) != same_headers.end()) {
      table_same_headers_positions.push_back(i);
    }
  }

  return {current_table_same_headers_positions, table_same_headers_positions};
}

void UnionProcessor::combine_table(std::shared_ptr<ResultTable> table) {
  if (!current_table) {
    current_table = std::move(table);
    init_headers_set();
    return;
  }

  auto extra_headers = select_extra_headers(*table);
  adjust_current_table(extra_headers);

  auto extra_headers_positions =
      find_extra_headers_positions(*table, extra_headers);

  auto [current_table_same_headers_positions, table_same_headers_positions] =
      find_same_headers_positions(*table, extra_headers);

  auto &table_data = table->data;
  while (!table_data.empty()) {
    auto row = table_data.front();
    current_table->data.push_back(as_current_table(
        row, extra_headers_positions, current_table_same_headers_positions,
        table_same_headers_positions));
    table_data.pop_front();
  }
}

std::set<unsigned long>
UnionProcessor::select_extra_headers(const ResultTable &table) const {
  std::set<unsigned long> extra_headers;
  std::set<unsigned long> current_headers(current_table->headers.begin(),
                                          current_table->headers.end());
  std::set<unsigned long> table_headers(table.headers.begin(),
                                        table.headers.end());

  std::set_difference(current_headers.begin(), current_headers.end(),
                      table_headers.begin(), table_headers.end(),
                      std::inserter(extra_headers, extra_headers.begin()));

  return extra_headers;
}

void UnionProcessor::adjust_current_table(
    const std::set<unsigned long> &extra_headers) {

  for (auto value : extra_headers) {
    current_table->headers.push_back(value);
  }

  for (auto &row : current_table->data) {
    for (size_t i = 0; i < extra_headers.size(); i++) {
      row.push_back(0);
    }
  }
}

void UnionProcessor::init_headers_set() {
  current_headers = get_headers_set(current_table->headers);
}

std::set<unsigned long> UnionProcessor::get_headers_set(
    const std::vector<unsigned long> &headers_list) {
  return std::set<unsigned long>(headers_list.begin(), headers_list.end());
}

void UnionProcessor::validate_table(const ResultTable &table) {

  if (table.headers.size() != current_table->headers.size()) {
    throw std::runtime_error("UnionProcessor::combine_table: Header list with "
                             "diferent sizes for tables");
  }

  auto table_headers_set = get_headers_set(table.headers);
  auto intersection = intersect_ul_sets(current_headers, table_headers_set);

  if (intersection != current_headers) {
    throw std::runtime_error(
        "UnionProcessor::combine_table: Invalid header set values for tables");
  }
}
