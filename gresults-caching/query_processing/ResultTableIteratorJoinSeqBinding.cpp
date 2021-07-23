//
// Created by cristobal on 21-07-21.
//

#include "ResultTableIteratorJoinSeqBinding.hpp"
#include "QueryProcessor.hpp"
bool ResultTableIteratorJoinSeqBinding::has_next() { return next_available; }

std::vector<unsigned long> ResultTableIteratorJoinSeqBinding::next() {
  return next_concrete();
}
std::vector<unsigned long> &ResultTableIteratorJoinSeqBinding::get_headers() {
  return headers;
}
void ResultTableIteratorJoinSeqBinding::reset_iterator() {
  input_it->reset_iterator();
  left_row_active = false;
  current_var_binding_qproc = nullptr;
  next_available = false;
  next_concrete();
}

ResultTableIteratorJoinSeqBinding::ResultTableIteratorJoinSeqBinding(
    std::shared_ptr<ResultTableIterator> input_it,
    const proto_msg::SparqlNode &proto_node,
    std::shared_ptr<VarBindingQProc> var_binding_qproc,
    std::set<unsigned long> &right_table_vars, TimeControl &time_control,
    std::shared_ptr<PredicatesCacheManager> cache_manager,
    std::shared_ptr<VarIndexManager> vim,
    std::shared_ptr<NaiveDynamicStringDictionary> extra_str_dict)
    : ResultTableIterator(time_control), input_it(std::move(input_it)),
      proto_node(proto_node), var_binding_qproc(std::move(var_binding_qproc)),
      current_var_binding_qproc(nullptr), current_right_it(nullptr),
      next_available(false), left_row_active(false),
      cache_manager(std::move(cache_manager)), vim(std::move(vim)),
      extra_str_dict(std::move(extra_str_dict)) {

  build_headers(right_table_vars);
  next_concrete();
}

void ResultTableIteratorJoinSeqBinding::build_headers(
    std::set<unsigned long> &right_table_vars) {
  std::set<unsigned long> headers_set(right_table_vars);
  auto &input_headers = input_it->get_headers();
  for (auto header : input_it->get_headers()) {
    headers_set.insert(header);
  }
  headers = std::vector<unsigned long>(headers_set.begin(), headers_set.end());

  left_header_pos_map = get_pos_map(input_headers, headers);

  tmp_holder = std::vector<unsigned long>(headers.size(), 0);
}
bool ResultTableIteratorJoinSeqBinding::create_var_binding_qproc_if_needed(
    std::vector<unsigned long> &left_row) {
  bool changed = false;

  if (!current_var_binding_qproc) {
    current_var_binding_qproc =
        std::make_shared<VarBindingQProc>(*var_binding_qproc);
    changed = true;
  }
  auto &input_headers = input_it->get_headers();

  if (!changed) {
    for (size_t i = 0; i < input_headers.size(); i++) {
      auto header_val = input_headers[i];
      auto left_val = left_row[i];
      if (!current_var_binding_qproc->is_bound(header_val) ||
          current_var_binding_qproc->get_value(header_val) != left_val) {
        // current_var_binding_qproc->bind(header_val, left_val);
        changed = true;
        break;
      }
    }
    if (changed) {
      current_var_binding_qproc =
          std::make_shared<VarBindingQProc>(*var_binding_qproc);
    }
  }

  if (changed) {
    for (size_t i = 0; i < input_headers.size(); i++) {
      auto header_val = input_headers[i];
      auto left_val = left_row[i];
      current_var_binding_qproc->bind(header_val, left_val);
    }
  }

  return changed;
}
std::vector<unsigned long> ResultTableIteratorJoinSeqBinding::next_concrete() {
  if (!time_control.tick())
    return next_value;
  next_available = false;
  auto result = next_value;

  if (!left_row_active) {
    for (;;) {
      if (!input_it->has_next()) {
        return result;
      }
      current_left_row = input_it->next();
      if (!time_control.tick())
        return result;
      auto changed_bindings =
          create_var_binding_qproc_if_needed(current_left_row);

      if (changed_bindings || !current_right_it) {
        current_right_it =
            QueryProcessor(cache_manager, vim, extra_str_dict, time_control)
                .run_query(proto_node, current_var_binding_qproc)
                .get_it_shared();
        if (!time_control.tick())
          return result;
        right_header_pos_map =
            get_pos_map(current_right_it->get_headers(), headers);
      }
      if (current_right_it->has_next()) {
        left_row_active = true;
        break;
      }
    }
  }

  if (current_right_it->has_next()) {
    next_available = true;
    auto right_row = current_right_it->next();
    if (!time_control.tick())
      return result;
    map_rows_to_holder(current_left_row, left_header_pos_map);
    map_rows_to_holder(right_row, right_header_pos_map);
    next_value = tmp_holder;
    if (!current_right_it->has_next()) {
      left_row_active = false;
    }
  }

  return result;
}
std::vector<unsigned long>
ResultTableIteratorJoinSeqBinding::get_pos_map(std::vector<unsigned long> &from,
                                               std::vector<unsigned long> &to) {
  auto result = std::vector<unsigned long>(from.size(), 0);
  for (size_t i = 0; i < from.size(); i++) {
    for (size_t j = 0; j < to.size(); j++) {
      if (from[i] == to[j]) {
        result[i] = j;
        break;
      }
    }
  }
  return result;
}
void ResultTableIteratorJoinSeqBinding::map_rows_to_holder(
    std::vector<unsigned long> &source_data,
    std::vector<unsigned long> &headers_mapping) {
  for (size_t i = 0; i < source_data.size(); i++) {
    auto pos_in_header = headers_mapping[i];
    tmp_holder[pos_in_header] = source_data[i];
  }
}
