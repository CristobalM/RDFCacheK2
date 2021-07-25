//
// Created by cristobal on 21-07-21.
//

#include "ResultTableIteratorLeftOuterJoin.hpp"
#include "QueryProcessor.hpp"
bool ResultTableIteratorLeftOuterJoin::has_next() { return next_available; }

std::vector<unsigned long> ResultTableIteratorLeftOuterJoin::next() {
  return next_concrete();
}
std::vector<unsigned long> &ResultTableIteratorLeftOuterJoin::get_headers() {
  return headers;
}
void ResultTableIteratorLeftOuterJoin::reset_iterator() {
  left_it->reset_iterator();
  left_row_active = false;
  current_var_binding_qproc = nullptr;
  next_available = false;
  next_concrete();
}

ResultTableIteratorLeftOuterJoin::ResultTableIteratorLeftOuterJoin(
    std::shared_ptr<ResultTableIterator> left_it,
    proto_msg::SparqlNode right_node,
    std::shared_ptr<VarBindingQProc> var_binding_qproc,
    std::set<unsigned long> &right_table_vars, TimeControl &time_control,
    std::shared_ptr<PredicatesCacheManager> cache_manager,
    std::shared_ptr<VarIndexManager> vim,
    std::shared_ptr<NaiveDynamicStringDictionary> extra_str_dict)
    : ResultTableIterator(time_control), left_it(std::move(left_it)),
      right_node(std::move(right_node)),
      var_binding_qproc(std::move(var_binding_qproc)),
      current_var_binding_qproc(nullptr), current_right_it(nullptr),
      next_available(false), left_row_active(false),
      cache_manager(std::move(cache_manager)), vim(std::move(vim)),
      extra_str_dict(std::move(extra_str_dict)) {

  build_headers(right_table_vars);
  next_concrete();
}

void ResultTableIteratorLeftOuterJoin::build_headers(
    std::set<unsigned long> &right_table_vars) {
  std::set<unsigned long> headers_set(right_table_vars);
  auto &input_headers = left_it->get_headers();
  for (auto header : left_it->get_headers()) {
    headers_set.insert(header);
  }
  headers = std::vector<unsigned long>(headers_set.begin(), headers_set.end());

  left_header_pos_map = get_pos_map(input_headers, headers);

  tmp_holder = std::vector<unsigned long>(headers.size(), 0);
  right_positions = build_right_positions(left_it->get_headers());
}
bool ResultTableIteratorLeftOuterJoin::create_var_binding_qproc_if_needed(
    std::vector<unsigned long> &left_row) {
  bool changed = false;

  if (!current_var_binding_qproc) {
    current_var_binding_qproc =
        std::make_shared<VarBindingQProc>(*var_binding_qproc);
    changed = true;
  }
  auto &input_headers = left_it->get_headers();

  if (!changed) {
    for (size_t i = 0; i < input_headers.size(); i++) {
      auto header_val = input_headers[i];
      auto left_val = left_row[i];
      if (!current_var_binding_qproc->is_bound(header_val) ||
          current_var_binding_qproc->get_value(header_val) != left_val) {
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
std::vector<unsigned long> ResultTableIteratorLeftOuterJoin::next_concrete() {
  if (!time_control.tick())
    return next_value;
  next_available = false;
  auto result = next_value;

  if (!left_row_active) {
    for (;;) {
      if (!left_it->has_next()) {
        return result;
      }
      current_left_row = left_it->next();
      if (!time_control.tick())
        return result;
      auto changed_bindings =
          create_var_binding_qproc_if_needed(current_left_row);

      if (changed_bindings || !current_right_it) {
        current_right_it =
            QueryProcessor(cache_manager, vim, extra_str_dict, time_control)
                .run_query(right_node, current_var_binding_qproc)
                .get_it_shared();
        if (!time_control.tick())
          return result;
        right_header_pos_map =
            get_pos_map(current_right_it->get_headers(), headers);
      }
      if (current_right_it->has_next()) {
        left_row_active = true;
        break;
      } else {
        next_available = true;
        map_rows_to_holder(current_left_row, left_header_pos_map);
        fill_right_rows_with_zero();
        next_value = tmp_holder;
        return result;
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
ResultTableIteratorLeftOuterJoin::get_pos_map(std::vector<unsigned long> &from,
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
void ResultTableIteratorLeftOuterJoin::map_rows_to_holder(
    std::vector<unsigned long> &source_data,
    std::vector<unsigned long> &headers_mapping) {
  for (size_t i = 0; i < source_data.size(); i++) {
    auto pos_in_header = headers_mapping[i];
    tmp_holder[pos_in_header] = source_data[i];
  }
}
void ResultTableIteratorLeftOuterJoin::fill_right_rows_with_zero() {
  for (auto p : right_positions) {
    tmp_holder[p] = 0;
  }
}
std::vector<unsigned long>
ResultTableIteratorLeftOuterJoin::build_right_positions(
    std::vector<unsigned long> &left_headers) {
  std::vector<unsigned long> result;
  std::set<unsigned long> left_headers_set(left_headers.begin(),
                                           left_headers.end());
  for (size_t i = 0; i < headers.size(); i++) {
    if (left_headers_set.find(headers[i]) == left_headers_set.end()) {
      result.push_back(i);
    }
  }
  return result;
}
