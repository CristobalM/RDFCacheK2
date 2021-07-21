//
// Created by cristobal on 7/14/21.
//

#include "LWRHMapJoinLazyBaseProcessor.hpp"
#include <TimeControl.hpp>

LWRHMapJoinLazyBaseProcessor::LWRHMapJoinLazyBaseProcessor(
    TimeControl &time_control, std::shared_ptr<ResultTableIterator> left_it,
    std::shared_ptr<ResultTableIterator> right_it)
    : time_control(time_control), left_it(std::move(left_it)),
      right_it(std::move(right_it)), join_vars(find_join_vars()),
      header_values(build_header()), header_positions(build_header_positions()),
      join_vars_real_positions(build_join_vars_real_positions()),
      right_hmap(build_right_hmap()) {
  build_to_result_vec_maps();
}

std::set<unsigned long> LWRHMapJoinLazyBaseProcessor::find_join_vars() const {
  auto &left_headers = left_it->get_headers();
  auto &right_headers = right_it->get_headers();
  std::set<unsigned long> left_headers_set(left_headers.begin(),
                                           left_headers.end());
  std::set<unsigned long> right_headers_set(right_headers.begin(),
                                            right_headers.end());
  std::set<unsigned long> result;
  std::set_intersection(left_headers_set.begin(), left_headers_set.end(),
                        right_headers_set.begin(), right_headers_set.end(),
                        std::inserter(result, result.begin()));
  return result;
}
void LWRHMapJoinLazyBaseProcessor::select_values(
    std::vector<unsigned long> &mutable_keys,
    std::vector<unsigned long> &mutable_values,
    const std::vector<unsigned long> &row,
    const std::vector<unsigned long> &join_var_positions_right,
    const std::vector<unsigned long> &non_join_var_positions_right) const {

  for (size_t i = 0; i < join_var_positions_right.size(); i++) {
    mutable_keys[i] = row[join_var_positions_right[i]];
  }
  for (size_t i = 0; i < non_join_var_positions_right.size(); i++) {
    mutable_values[i] = row[non_join_var_positions_right[i]];
  }
}
LWRHMapJoinLazyBaseProcessor::jmap_t
LWRHMapJoinLazyBaseProcessor::build_right_hmap() {
  jmap_t result;

  auto [right_table_join_var_positions, right_table_non_join_var_positions] =
      find_join_nojoin_vars_in_vec(right_it->get_headers());

  std::vector<unsigned long> key_holder(join_vars.size(), 0);
  std::vector<unsigned long> value_holder(
      right_table_non_join_var_positions.size(), 0);

  while (right_it->has_next()) {
    auto row = right_it->next();
    select_values(key_holder, value_holder, row, right_table_join_var_positions,
                  right_table_non_join_var_positions);
    result[key_holder].push_back(value_holder);
    if (!time_control.tick())
      return result;
  }

  return result;
}
std::vector<unsigned long> LWRHMapJoinLazyBaseProcessor::build_header() {
  std::set<unsigned long> result;

  auto &left_headers = left_it->get_headers();
  auto &right_headers = right_it->get_headers();

  for (auto h : left_headers)
    result.insert(h);
  for (auto h : right_headers)
    result.insert(h);

  return std::vector<unsigned long>(result.begin(), result.end());
}
std::unordered_map<unsigned long, unsigned long>
LWRHMapJoinLazyBaseProcessor::build_header_positions() {
  std::unordered_map<unsigned long, unsigned long> result;
  for (size_t i = 0; i < header_values.size(); i++) {
    result[header_values[i]] = i;
  }
  return result;
}
std::vector<unsigned long>
LWRHMapJoinLazyBaseProcessor::build_join_vars_real_positions() {
  std::vector<unsigned long> result;
  for (auto join_var : join_vars) {
    result.push_back(header_positions[join_var]);
  }
  return result;
}
std::pair<std::vector<unsigned long>, std::vector<unsigned long>>
LWRHMapJoinLazyBaseProcessor::find_join_nojoin_vars_in_vec(
    std::vector<unsigned long> &headers_vec) {
  std::vector<unsigned long> join_vars_pos;
  std::vector<unsigned long> non_join_vars_pos;

  for (size_t i = 0; i < headers_vec.size(); i++) {
    auto header_value = headers_vec[i];
    if (join_vars.find(header_value) != join_vars.end()) {
      join_vars_pos.push_back(i);
    } else {
      non_join_vars_pos.push_back(i);
    }
  }

  return {std::move(join_vars_pos), std::move(non_join_vars_pos)};
}
void LWRHMapJoinLazyBaseProcessor::build_to_result_vec_maps() {
  auto &left_header = left_it->get_headers();
  left_headers_to_result = std::vector<unsigned long>(left_header.size(), 0);

  std::set<unsigned long> left_headers_set(left_header.begin(),
                                           left_header.end());

  for (size_t h = 0; h < header_values.size(); h++) {
    for (size_t l = 0; l < left_header.size(); l++) {
      if (left_header[l] == header_values[h]) {
        left_headers_to_result[l] = h;
      }
    }
  }
  auto &right_header = right_it->get_headers();
  std::vector<unsigned long> non_join_right_header_values;
  for (unsigned long rh : right_header) {
    if (join_vars.find(rh) == join_vars.end()) {
      non_join_right_header_values.push_back(rh);
    }
  }
  right_headers_positions_in_final =
      std::vector<unsigned long>(non_join_right_header_values.size(), 0);

  for (size_t r = 0; r < non_join_right_header_values.size(); r++) {
    for (size_t h = 0; h < header_values.size(); h++) {
      if (non_join_right_header_values[r] == header_values[h]) {
        right_headers_positions_in_final[r] = h;
      }
    }
  }
}
