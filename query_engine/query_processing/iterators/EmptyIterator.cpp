//
// Created by cristobal on 7/15/21.
//

#include "EmptyIterator.hpp"
#include <stdexcept>
bool EmptyIterator::has_next() { return false; }
std::vector<unsigned long> EmptyIterator::next() {
  throw std::runtime_error("calling next on EmptyIterator is not allowed");
}
std::vector<unsigned long> &EmptyIterator::get_headers() { return headers; }
void EmptyIterator::reset_iterator() {}
EmptyIterator::EmptyIterator(std::vector<unsigned long> headers,
                             TimeControl &time_control)
    : QueryIterator(time_control), headers(std::move(headers)) {}
EmptyIterator::EmptyIterator(TimeControl &time_control)
    : QueryIterator(time_control) {}
