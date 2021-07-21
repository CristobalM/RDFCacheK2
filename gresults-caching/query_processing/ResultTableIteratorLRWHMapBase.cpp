//
// Created by cristobal on 7/14/21.
//

#include "ResultTableIteratorLRWHMapBase.hpp"
#include "ResultTableIteratorLeftOuterJoin.hpp"
#include <TimeControl.hpp>
bool ResultTableIteratorLRWHMapBase::has_next() { return next_available; }
std::vector<unsigned long> &ResultTableIteratorLRWHMapBase::get_headers() {
  return headers;
}
void ResultTableIteratorLRWHMapBase::reset_iterator() {
  left_it->reset_iterator();
  next_available = false;
  next();
}
void ResultTableIteratorLRWHMapBase::put_join_values_in_holder(
    std::vector<unsigned long> &row) {
  for (size_t i = 0; i < join_vars_positions.size(); i++) {
    key_holder[i] = row[join_vars_positions[i]];
  }
}
void ResultTableIteratorLRWHMapBase::put_values_in_result(
    std::vector<unsigned long> &left_values,
    std::vector<unsigned long> &right_values) {
  for (size_t i = 0; i < left_values.size(); i++) {
    result_holder[left_headers_to_result[i]] = left_values[i];
  }
  for (size_t i = 0; i < right_values.size(); i++) {
    result_holder[right_values_to_result[i]] = right_values[i];
  }
}
void ResultTableIteratorLRWHMapBase::put_left_values_in_result(
    std::vector<unsigned long> &left_values) {
  for (size_t i = 0; i < left_values.size(); i++) {
    result_holder[left_headers_to_result[i]] = left_values[i];
  }
  for (unsigned long rpos : right_values_to_result) {
    result_holder[rpos] = 0;
  }
}
ResultTableIteratorLRWHMapBase::ResultTableIteratorLRWHMapBase(
    std::vector<unsigned long> &&headers,
    std::vector<unsigned long> &&join_vars_positions,
    std::shared_ptr<ResultTableIterator> left_it,
    std::unordered_map<std::vector<unsigned long>,
                       std::vector<std::vector<unsigned long>>, fnv_hash_64>
        &&right_hmap,
    std::vector<unsigned long> &&left_headers_to_result,
    std::vector<unsigned long> &&right_values_to_result,
    TimeControl &time_control)
    : ResultTableIterator(time_control),
      key_holder(join_vars_positions.size(), 0),
      result_holder(headers.size(), 0), headers(std::move(headers)),
      join_vars_positions(std::move(join_vars_positions)),
      left_it(std::move(left_it)), right_hmap(std::move(right_hmap)),
      left_headers_to_result(std::move(left_headers_to_result)),
      right_values_to_result(std::move(right_values_to_result)),
      next_available(false) {}
void ResultTableIteratorLRWHMapBase::extract_next() {
  auto &current_right_values = current_matched_values.front();
  put_values_in_result(current_left_row, current_right_values);
  current_value = result_holder;
  current_matched_values.pop_front();
  next_available = true;
}