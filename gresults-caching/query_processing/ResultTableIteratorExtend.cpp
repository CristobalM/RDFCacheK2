//
// Created by cristobal on 7/14/21.
//

#include "ResultTableIteratorExtend.hpp"
ResultTableIteratorExtend::ResultTableIteratorExtend(
    std::shared_ptr<ResultTableIterator> input_it,
    std::unique_ptr<EvalData> &&eval_data,
    std::vector<std::unique_ptr<VarLazyBinding>> &&var_bindings)
    : input_it(std::move(input_it)), eval_data(std::move(eval_data)),
      var_bindings(std::move(var_bindings)), headers(build_headers()),
      mapping_original(build_mapping_original()),
      mapping_extra(build_mapping_extra()) {}
bool ResultTableIteratorExtend::has_next() { return input_it->has_next(); }
std::vector<unsigned long> ResultTableIteratorExtend::next() {
  return next_concrete();
}
std::vector<unsigned long> &ResultTableIteratorExtend::get_headers() {
  return headers;
}
void ResultTableIteratorExtend::reset_iterator() { input_it->reset_iterator(); }
std::vector<unsigned long> ResultTableIteratorExtend::build_headers() {
  auto &original_headers = input_it->get_headers();
  std::set<unsigned long> headers_set(original_headers.begin(),
                                      original_headers.end());
  for (auto &vb : var_bindings) {
    headers_set.insert(vb->get_var_id());
  }
  return std::vector<unsigned long>(headers_set.begin(), headers_set.end());
}
std::vector<unsigned long> ResultTableIteratorExtend::next_concrete() {
  auto next_row_pre = input_it->next();
  auto next_row = get_transformed_row(next_row_pre);
  return next_row;
}
std::vector<unsigned long> ResultTableIteratorExtend::build_mapping_original() {
  std::vector<unsigned long> result;
  auto &original_headers = input_it->get_headers();
  std::set<unsigned long> headers_set(original_headers.begin(),
                                      original_headers.end());
  for (size_t i = 0; i < headers.size(); i++) {
    auto h = headers[i];
    if (headers_set.find(h) != headers_set.end()) {
      result.push_back(i);
    }
  }
  return result;
}
std::vector<unsigned long> ResultTableIteratorExtend::build_mapping_extra() {
  std::vector<unsigned long> result;
  std::set<unsigned long> headers_set;
  for (auto &vb : var_bindings) {
    headers_set.insert(vb->get_var_id());
  }
  for (size_t i = 0; i < headers.size(); i++) {
    auto h = headers[i];
    if (headers_set.find(h) != headers_set.end()) {
      result.push_back(i);
    }
  }
  return result;
}
std::vector<unsigned long> ResultTableIteratorExtend::get_transformed_row(
    std::vector<unsigned long> &input_row) {
  std::vector<unsigned long> result(headers.size(), 0);
  for (size_t i = 0; i < mapping_original.size(); i++) {
    result[mapping_original[i]] = input_row[i];
  }
  for (size_t i = 0; i < var_bindings.size(); i++) {
    auto &vb = var_bindings[i];
    auto resource_evaluated = vb->get_evaluator().produce_resource(input_row);
    auto rdf_resource = resource_evaluated->get_resource_clone();
    auto rdf_resource_id_value = get_rdf_resource_id_value(rdf_resource);
    result[mapping_extra[i]] = rdf_resource_id_value;
  }
  return result;
}
unsigned long
ResultTableIteratorExtend::get_rdf_resource_id_value(RDFResource &resource) {
  auto &cm = eval_data->cm;
  auto &extra_str_dict = eval_data->extra_dict;
  auto resource_id = cm.get_resource_index(resource);

  if (resource_id == 0) {
    auto extra_dict_value = extra_str_dict.locate_resource(resource);
    if (extra_dict_value == 0) {
      extra_str_dict.add_resource(resource);
      extra_dict_value = extra_str_dict.locate_resource(resource);
    }
    resource_id = extra_dict_value + cm.get_last_id();
  }
  return resource_id;
}
