//
// Created by cristobal on 7/21/21.
//

#include "CrossProductIterator.hpp"
#include <set>
bool CrossProductIterator::has_next() { return next_available; }
std::vector<unsigned long> CrossProductIterator::next() {
  return next_concrete();
}
std::vector<unsigned long> &CrossProductIterator::get_headers() {
  return headers;
}
void CrossProductIterator::reset_iterator() {
  left_it->reset_iterator();
  right_it->reset_iterator();
  left_row_active = false;
  next_available = false;
  next_concrete();
}
CrossProductIterator::CrossProductIterator(
    std::shared_ptr<QueryIterator> left_it,
    std::shared_ptr<QueryIterator> right_it, TimeControl &time_control)
    : QueryIterator(time_control), left_it(std::move(left_it)),
      right_it(std::move(right_it)), next_available(false),
      left_row_active(false) {
  build_headers();
  next_concrete();
}
void CrossProductIterator::build_headers() {
  auto &left_headers = left_it->get_headers();
  auto &right_headers = right_it->get_headers();
  headers.insert(headers.end(), left_headers.begin(), left_headers.end());
  headers.insert(headers.end(), right_headers.begin(), right_headers.end());
  tmp_holder = std::vector<unsigned long>(headers.size(), 0);
}
std::vector<unsigned long> CrossProductIterator::next_concrete() {
  next_available = false;
  if (!time_control.tick())
    return next_result;
  auto result = next_result;

  if (!left_row_active) {
    for (;;) {
      if (!left_it->has_next())
        return result;
      current_left_row = left_it->next();
      if (!time_control.tick())
        return result;
      if (right_it->has_next()) {
        left_row_active = true;
        break;
      }
    }
  }

  auto right_row = right_it->next();
  if (!time_control.tick())
    return result;

  next_available = true;
  map_values_to_holder(right_row);
  next_result = tmp_holder;
  if (!right_it->has_next()) {
    left_row_active = false;
  }
  return result;
}
void CrossProductIterator::map_values_to_holder(
    std::vector<unsigned long> &right_row) {
  for (size_t i = 0; i < current_left_row.size(); i++) {
    tmp_holder[i] = current_left_row[i];
  }
  for (size_t i = current_left_row.size(); i < tmp_holder.size(); i++) {
    tmp_holder[i] = right_row[i - current_left_row.size()];
  }
}
