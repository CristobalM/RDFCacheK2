
#include "UnionProcessor.hpp"

#include <algorithm>
#include <stdexcept>

UnionProcessor::UnionProcessor(VarIndexManager &vim)
    : vim(vim), current_table(nullptr) {}

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

void UnionProcessor::combine_table(std::shared_ptr<ResultTable> table) {
  if (!current_table) {
    current_table = std::move(table);
    init_headers_set();
    return;
  }

  validate_table(*table);
  auto table_permutation = find_table_permutation(*table);

  auto &table_data = table->data;
  while (!table_data.empty()) {
    auto row = table_data.front();
    current_table->data.push_back(permutate_row(table_permutation, row));
    table_data.pop_front();
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

std::vector<unsigned long>
UnionProcessor::find_table_permutation(const ResultTable &table) {
  std::vector<unsigned long> table_permutation(table.headers.size(), 0);
  std::vector<unsigned long> table_permutation_inverse(table.headers.size(), 0);
  std::unordered_map<unsigned long, unsigned long> curr_header_inv_map;

  for (unsigned long i = 0; i < current_table->headers.size(); i++) {
    curr_header_inv_map[current_table->headers[i]] = i;
  }

  for (unsigned long i = 0; i < table.headers.size(); i++) {
    table_permutation[i] = curr_header_inv_map[table.headers[i]];
  }

  for (unsigned long i = 0; i < table_permutation.size(); i++) {
    table_permutation_inverse[table_permutation[i]] = i;
  }

  return table_permutation_inverse;
}

std::vector<unsigned long>
permutate_row(const std::vector<unsigned long> &table_permutation,
              std::vector<unsigned long> &input_row) {
  std::vector<unsigned long> result(input_row.size(), 0);
  for (size_t i = 0; i < input_row.size(); i++) {
    result[i] = input_row[table_permutation[i]];
  }
  return result;
}
