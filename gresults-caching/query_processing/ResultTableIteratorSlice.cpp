//
// Created by cristobal on 15-07-21.
//

#include "ResultTableIteratorSlice.hpp"
bool ResultTableIteratorSlice::has_next() {
  return input_it->has_next() && current_position < start + length;
}
std::vector<unsigned long> ResultTableIteratorSlice::next() {
  current_position++;
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
    std::shared_ptr<ResultTableIterator> input_it, unsigned long start,
    unsigned long length)
    : input_it(std::move(input_it)), start(start), length(length),
      current_position(0) {
  for (; current_position < start; current_position++) {
    if (this->input_it->has_next())
      this->input_it->next();
    else
      break;
  }
}
