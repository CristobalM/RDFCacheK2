//
// Created by cristobal on 7/15/21.
//

#include "ResultTableIteratorEmpty.hpp"
#include <stdexcept>
bool ResultTableIteratorEmpty::has_next() { return false; }
std::vector<unsigned long> ResultTableIteratorEmpty::next() {
  throw std::runtime_error(
      "calling next on ResultTableIteratorEmpty is not allowed");
}
std::vector<unsigned long> &ResultTableIteratorEmpty::get_headers() {
  return headers;
}
void ResultTableIteratorEmpty::reset_iterator() {}
ResultTableIteratorEmpty::ResultTableIteratorEmpty(
    std::vector<unsigned long> headers)
    : headers(std::move(headers)) {}
