//
// Created by cristobal on 7/14/21.
//

#include "UnionIterator.hpp"
#include <TimeControl.hpp>
#include <set>
UnionIterator::UnionIterator(
    std::vector<std::shared_ptr<QueryIterator>> &&iterators,
    TimeControl &time_control)
    : QueryIterator(time_control), iterators(std::move(iterators)),
      headers(build_headers()), headers_mapping(build_headers_mapping()),
      current_it_pos(0) {}
bool UnionIterator::has_next() {
  if (current_it_pos >= iterators.size())
    return false;
  return iterators[current_it_pos]->has_next();
}
std::vector<unsigned long> UnionIterator::next() { return next_concrete(); }
std::vector<unsigned long> &UnionIterator::get_headers() { return headers; }
void UnionIterator::reset_iterator() {
  for (auto &it : iterators)
    it->reset_iterator();
  current_it_pos = 0;
}
std::vector<unsigned long> UnionIterator::next_concrete() {
  auto &it = iterators[current_it_pos];
  auto next_row_pre = it->next();
  if (!time_control.tick())
    return std::vector<unsigned long>();
  auto next_row = get_mapped_row(next_row_pre, current_it_pos);
  if (!it->has_next()) {
    current_it_pos++;
  }
  return next_row;
}
std::vector<unsigned long> UnionIterator::build_headers() {
  std::set<unsigned long> headers_set;
  for (auto &it : iterators) {
    for (auto h : it->get_headers()) {
      headers_set.insert(h);
    }
  }
  return std::vector<unsigned long>(headers_set.begin(), headers_set.end());
}
std::vector<std::vector<unsigned long>> UnionIterator::build_headers_mapping() {
  std::vector<std::vector<unsigned long>> result;

  for (auto &it : iterators) {
    auto &curr_headers = it->get_headers();
    std::vector<unsigned long> curr_mapping(curr_headers.size(), 0);
    for (size_t i = 0; i < curr_headers.size(); i++) {
      for (size_t j = 0; j < headers.size(); j++) {
        if (headers[j] == curr_headers[i]) {
          curr_mapping[i] = j;
        }
      }
    }
    result.push_back(std::move(curr_mapping));
  }

  return result;
}
std::vector<unsigned long>
UnionIterator::get_mapped_row(std::vector<unsigned long> &input_row,
                              size_t it_pos) {
  std::vector<unsigned long> result(headers.size(), 0);
  auto &mapping = headers_mapping[it_pos];
  for (size_t i = 0; i < mapping.size(); i++) {
    result[mapping[i]] = input_row[i];
  }
  return result;
}
