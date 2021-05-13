//
// Created by cristobal on 5/12/21.
//

#include "InnerJoinProcessor.hpp"

#include <algorithm>

InnerJoinProcessor::InnerJoinProcessor(std::shared_ptr<ResultTable> left_table,
                                       std::shared_ptr<ResultTable> right_table)
    : left_table(std::move(left_table)), right_table(std::move(right_table)) {}

std::vector<unsigned long> InnerJoinProcessor::get_diff_list(
    const std::vector<unsigned long> &all_values,
    const std::set<unsigned long> &values_to_remove) const {
  std::vector<unsigned long> result;
  for (auto v : all_values) {
    if (values_to_remove.find(v) == values_to_remove.end())
      result.push_back(v);
  }
  return result;
}

std::shared_ptr<ResultTable> InnerJoinProcessor::execute() {

  auto join_vars = get_join_vars();
  auto rt_real_indexes = get_real_indexes(right_table->headers, join_vars);
  auto join_map = get_join_map(rt_real_indexes);

  auto lt_real_indexes = get_real_indexes(left_table->headers, join_vars);
  std::vector<unsigned long> values(lt_real_indexes.size(), 0);

  auto right_only_headers = get_diff_list(right_table->headers, join_vars);

  for (auto h : right_only_headers) {
    left_table->headers.push_back(h);
  }

  for (auto it = left_table->data.begin(); it != left_table->data.end();) {
    select_values(values, *it, lt_real_indexes);
    auto next_it = std::next(it);
    if (join_map.find(values) != join_map.end()) {
      join_values(it, join_map[values]);
    } else {
      left_table->data.erase(it);
    }
    it = next_it;
  }
  return left_table;
}

std::vector<unsigned long> InnerJoinProcessor::get_extra_cols(
    const std::vector<unsigned long> &row,
    const std::vector<unsigned long> &to_exclude_positions) const {
  std::vector<unsigned long> result;
  for (size_t i = 0, e_j = 0; i < row.size(); i++) {
    if (i == to_exclude_positions[e_j]) {
      e_j++;
      continue;
    }
    result.push_back(row[i]);
  }
  return result;
}

InnerJoinProcessor::jmap_t InnerJoinProcessor::get_join_map(
    const std::vector<unsigned long> &join_vars_real_indexes) const {
  jmap_t result;
  std::vector<unsigned long> keys(join_vars_real_indexes.size(), 0);
  while (!right_table->data.empty()) {
    const auto &curr = right_table->data.front();
    for (size_t i = 0; i < join_vars_real_indexes.size(); i++) {
      keys[i] = curr[join_vars_real_indexes[i]];
    }

    auto extra_cols = get_extra_cols(curr, join_vars_real_indexes);

    result[keys].push_back(
        std::move(extra_cols)); // this copy is potentially slow

    right_table->data.pop_front();
  }

  for (const auto &row : right_table->data) {
    for (size_t i = 0; i < join_vars_real_indexes.size(); i++) {
      keys[i] = row[join_vars_real_indexes[i]];
    }
  }
  return result;
}

std::set<unsigned long> InnerJoinProcessor::get_join_vars() const {
  std::set<unsigned long> left_headers_set(left_table->headers.begin(),
                                           left_table->headers.end());
  std::set<unsigned long> right_headers_set(right_table->headers.begin(),
                                            right_table->headers.end());
  std::set<unsigned long> result;
  std::set_intersection(left_headers_set.begin(), left_headers_set.end(),
                        right_headers_set.begin(), right_headers_set.end(),
                        std::inserter(result, result.begin()));
  return result;
}

void InnerJoinProcessor::join_values(
    std::list<std::vector<unsigned long>>::iterator &it,
    const vvul_t &rows) const {
  auto next_it = std::next(it);

  for (const auto &row : rows) {
    auto lrow_copy = *it;
    for (auto value : row) {
      lrow_copy.push_back(value);
    }
    left_table->data.insert(next_it, lrow_copy);
  }

  left_table->data.erase(it);
}

std::vector<unsigned long> InnerJoinProcessor::get_real_indexes(
    const std::vector<unsigned long> &headers,
    const std::set<unsigned long> &join_vars) const {

  std::vector<unsigned long> result;
  for (unsigned long i = 0; i < static_cast<unsigned long>(headers.size());
       i++) {
    auto value = headers.at(i);
    if (join_vars.find(value) != join_vars.end()) {
      result.push_back(i);
    }
  }
  return result;
}

void InnerJoinProcessor::select_values(
    std::vector<unsigned long> &mutable_values,
    const std::vector<unsigned long> &row,
    const std::vector<unsigned long> &join_vars_real_indexes) const {
  for (size_t i = 0; i < join_vars_real_indexes.size(); i++) {
    mutable_values[i] = row[join_vars_real_indexes[i]];
  }
}
