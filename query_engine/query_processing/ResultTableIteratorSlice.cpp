//
// Created by cristobal on 15-07-21.
//

#include "ResultTableIteratorSlice.hpp"
#include <TimeControl.hpp>
#include <limits>
bool ResultTableIteratorSlice::has_next() {
  return input_it->has_next() && current_position < start + length;
}
std::vector<unsigned long> ResultTableIteratorSlice::next() {
  current_position++;
  time_control.tick();
  return input_it->next();
}
std::vector<unsigned long> &ResultTableIteratorSlice::get_headers() {
  return input_it->get_headers();
}
void ResultTableIteratorSlice::reset_iterator() {
  current_position = 0;
  input_it->reset_iterator();
}
ResultTableIteratorSlice::ResultTableIteratorSlice(
    std::shared_ptr<ResultTableIterator> input_it, long start, long length,
    TimeControl &time_control)
    : ResultTableIterator(time_control), input_it(std::move(input_it)),
      start(sanitize_start(start)), length(sanitize_length(length)),
      current_position(0) {
  for (; current_position < this->start; current_position++) {
    if (!time_control.tick())
      return;
    if (this->input_it->has_next())
      this->input_it->next();
    else
      break;
  }
}
long ResultTableIteratorSlice::sanitize_length(long value) {
  return value < 0 ? std::numeric_limits<long>::max() : value;
}
long ResultTableIteratorSlice::sanitize_start(long value) const {
  return value < 0 ? 0 : value;
}
