//
// Created by cristobal on 15-07-21.
//

#include "ResultTableIteratorInnerJoin.hpp"
std::vector<unsigned long> ResultTableIteratorInnerJoin::next() {
  return next_concrete();
}
ResultTableIteratorInnerJoin::ResultTableIteratorInnerJoin(
    std::vector<unsigned long> &&headers,
    std::vector<unsigned long> &&join_vars_positions,
    std::shared_ptr<ResultTableIterator> left_it,
    std::unordered_map<std::vector<unsigned long>,
                       std::vector<std::vector<unsigned long>>, fnv_hash_64>
        &&right_hmap,
    std::vector<unsigned long> &&left_headers_to_result,
    std::vector<unsigned long> &&right_values_to_result)
    : ResultTableIteratorLRWHMapBase(std::move(headers),
                                     std::move(join_vars_positions),
                                     std::move(left_it), std::move(right_hmap),
                                     std::move(left_headers_to_result),
                                     std::move(right_values_to_result)) {
  next_concrete();
}
std::vector<unsigned long> ResultTableIteratorInnerJoin::next_concrete() {
  auto result = current_value;
  next_available = false;
  while (current_matched_values.empty()) {
    if (!left_it->has_next()) {
      next_available = false;
      return result;
    }
    current_left_row = left_it->next();
    put_join_values_in_holder(current_left_row);
    auto right_vecs = right_hmap[key_holder];
    for (auto &v : right_vecs) {
      current_matched_values.push_back(std::move(v));
    }
  }
  extract_next();
  return result;
}
