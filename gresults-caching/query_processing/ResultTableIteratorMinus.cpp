//
// Created by cristobal on 7/14/21.
//

#include "ResultTableIteratorMinus.hpp"
#include <TimeControl.hpp>
bool ResultTableIteratorMinus::has_next() { return next_available; }
std::vector<unsigned long> ResultTableIteratorMinus::next() {
  return next_concrete();
}
std::vector<unsigned long> &ResultTableIteratorMinus::get_headers() {
  return left_it->get_headers();
}
void ResultTableIteratorMinus::reset_iterator() {
  left_it->reset_iterator();
  next_available = false;
  next_concrete();
}
ResultTableIteratorMinus::ResultTableIteratorMinus(
    std::shared_ptr<ResultTableIterator> left_it, ResultTableIterator &right_it,
    TimeControl &time_control)
    : ResultTableIterator(time_control), left_it(std::move(left_it)),
      right_index(build_right_index(right_it, time_control)),
      next_available(false), key_holder(right_it.get_headers().size(), 0),
      key_positions_left(build_key_positions_left(right_it.get_headers())) {
  next_concrete();
}
ResultTableIteratorMinus::set_t
ResultTableIteratorMinus::build_right_index(ResultTableIterator &right_it,
                                            TimeControl &time_control) {
  set_t result;
  while (right_it.has_next()) {
    auto current = right_it.next();
    if (!time_control.tick())
      return result;
    result.insert(std::move(current));
  }
  return result;
}
std::vector<unsigned long> ResultTableIteratorMinus::next_concrete() {
  auto result = next_result;
  next_available = false;

  while (left_it->has_next()) {
    auto current_lr = left_it->next();
    if (!time_control.tick())
      return result;

    select_key_values(current_lr);
    // then value is not on right, so it should be considered
    if (right_index.find(key_holder) == right_index.end()) {
      next_available = true;
      next_result = std::move(current_lr);
      return result;
    }
    // at this point the current_lr should be discarded, then continue until
    // there are no more items on the left
  }
  return result;
}
void ResultTableIteratorMinus::select_key_values(
    std::vector<unsigned long> &left_row) {
  for (size_t i = 0; i < key_positions_left.size(); i++) {
    key_holder[i] = left_row[key_positions_left[i]];
  }
}
std::vector<unsigned long> ResultTableIteratorMinus::build_key_positions_left(
    std::vector<unsigned long> &right_headers) {
  std::vector<unsigned long> result;
  auto &left_headers = left_it->get_headers();
  for (unsigned long right_header : right_headers) {
    for (size_t j = 0; j < left_headers.size(); j++) {
      if (left_headers[j] == right_header) {
        result.push_back(j);
        continue;
      }
    }
  }
  return result;
}
