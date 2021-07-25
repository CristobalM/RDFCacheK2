//
// Created by cristobal on 25-07-21.
//

#include "ResultTableIteratorMinus.hpp"
#include "QueryProcessor.hpp"
bool ResultTableIteratorMinus::has_next() { return next_available; }
std::vector<unsigned long> ResultTableIteratorMinus::next() {
  return next_concrete();
}
std::vector<unsigned long> &ResultTableIteratorMinus::get_headers() {
  return left_it->get_headers();
}
void ResultTableIteratorMinus::reset_iterator() { left_it->reset_iterator(); }

ResultTableIteratorMinus::ResultTableIteratorMinus(
    std::shared_ptr<ResultTableIterator> left_it,
    proto_msg::SparqlNode right_node,
    std::shared_ptr<VarBindingQProc> var_binding_qproc,
    TimeControl &time_control,
    std::shared_ptr<PredicatesCacheManager> cache_manager,
    std::shared_ptr<VarIndexManager> vim,
    std::shared_ptr<NaiveDynamicStringDictionary> extra_str_dict)
    : ResultTableIterator(time_control), left_it(std::move(left_it)),
      right_node(std::move(right_node)),
      var_binding_qproc(std::move(var_binding_qproc)), next_available(false),
      current_right_it(nullptr), cache_manager(std::move(cache_manager)),
      vim(std::move(vim)), extra_str_dict(std::move(extra_str_dict))

{
  next_concrete();
}

std::vector<unsigned long> ResultTableIteratorMinus::next_concrete() {
  if (!time_control.tick())
    return next_result;
  next_available = false;
  auto result = next_result;

  while (left_it->has_next()) {
    auto current_left_row = left_it->next();
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
    }

    if (!current_right_it->has_next()) {
      next_result = std::move(current_left_row);
      next_available = true;
      break;
    }
  }
  return result;
}

bool ResultTableIteratorMinus::create_var_binding_qproc_if_needed(
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
