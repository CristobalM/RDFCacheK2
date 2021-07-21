//
// Created by cristobal on 7/14/21.
//

#include "ResultTableIteratorLeftOuterJoin.hpp"
#include "ResultTableIteratorLRWHMapBase.hpp"
#include <TimeControl.hpp>
std::vector<unsigned long> ResultTableIteratorLeftOuterJoin::next() {
  return next_concrete();
}
ResultTableIteratorLeftOuterJoin::ResultTableIteratorLeftOuterJoin(
    std::vector<unsigned long> &&headers,
    std::vector<unsigned long> &&join_vars_positions,
    std::shared_ptr<ResultTableIterator> left_it,
    std::unordered_map<std::vector<unsigned long>,
                       std::vector<std::vector<unsigned long>>, fnv_hash_64>
        &&right_hmap,
    std::vector<unsigned long> &&left_headers_to_result,
    std::vector<unsigned long> &&right_values_to_result,
    TimeControl &time_control)
    : ResultTableIteratorLRWHMapBase(
          std::move(headers), std::move(join_vars_positions),
          std::move(left_it), std::move(right_hmap),
          std::move(left_headers_to_result), std::move(right_values_to_result),
          time_control) {
  next_concrete();
}
std::vector<unsigned long> ResultTableIteratorLeftOuterJoin::next_concrete() {
  if (!time_control.tick())
    return current_value;
  auto result = current_value;
  next_available = false;

  if (!current_matched_values.empty()) {
    extract_next();
    return result;
  }

  if (!left_it->has_next()) {
    next_available = false;
    return result;
  }

  current_left_row = left_it->next();
  if (!time_control.tick())
    return result;
  put_join_values_in_holder(current_left_row);
  auto right_vecs = right_hmap[key_holder];
  for (auto &v : right_vecs) {
    current_matched_values.push_back(std::move(v));
  }

  if (!current_matched_values.empty())
    extract_next();
  else {
    put_left_values_in_result(current_left_row);
    next_available = true;
    current_value = result_holder;
  }
  return result;
}
