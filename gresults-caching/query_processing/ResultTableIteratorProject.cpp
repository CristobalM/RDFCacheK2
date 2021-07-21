//
// Created by cristobal on 7/14/21.
//

#include "ResultTableIteratorProject.hpp"
#include <TimeControl.hpp>
bool ResultTableIteratorProject::has_next() { return input_it->has_next(); }
std::vector<unsigned long> ResultTableIteratorProject::next() {
  auto original_row = input_it->next();
  time_control.tick();
  for (size_t i = 0; i < tmp_holder.size(); i++) {
    tmp_holder[i] = original_row[vars_to_keep_position[i]];
  }
  return tmp_holder;
}
std::vector<unsigned long> &ResultTableIteratorProject::get_headers() {
  return headers;
}
void ResultTableIteratorProject::reset_iterator() {
  input_it->reset_iterator();
}
ResultTableIteratorProject::ResultTableIteratorProject(
    std::shared_ptr<ResultTableIterator> input_it,
    std::set<unsigned long> &vars_to_keep, TimeControl &time_control)
    : ResultTableIterator(time_control), input_it(std::move(input_it)),
      vars_to_keep_position(find_vars_to_keep_position(vars_to_keep)),
      headers(build_headers()), tmp_holder(headers.size(), 0) {}
std::vector<unsigned long>
ResultTableIteratorProject::find_vars_to_keep_position(
    std::set<unsigned long> &vars_to_keep) {
  std::vector<unsigned long> result;
  auto &original_headers = input_it->get_headers();
  for (size_t i = 0; i < original_headers.size(); i++) {
    if (vars_to_keep.find(original_headers[i]) != vars_to_keep.end()) {
      result.push_back(i);
    }
  }
  return result;
}
std::vector<unsigned long> ResultTableIteratorProject::build_headers() {
  std::vector<unsigned long> result;
  auto &original_headers = input_it->get_headers();
  for (auto p : vars_to_keep_position) {
    result.push_back(original_headers[p]);
  }
  return result;
}
