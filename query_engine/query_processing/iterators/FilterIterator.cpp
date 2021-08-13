//
// Created by cristobal on 7/14/21.
//

#include "FilterIterator.hpp"
#include <TimeControl.hpp>
#include <query_processing/ExprProcessor.hpp>
bool FilterIterator::has_next() { return next_available; }
std::vector<unsigned long> FilterIterator::next() { return next_concrete(); }
std::vector<unsigned long> &FilterIterator::get_headers() {
  return input_it->get_headers();
}
void FilterIterator::reset_iterator() {
  input_it->reset_iterator();
  next_concrete();
}
FilterIterator::FilterIterator(
    std::shared_ptr<QueryIterator> input_it, VarIndexManager &vim,
    std::vector<const proto_msg::ExprNode *> &expr_nodes,
    std::shared_ptr<PredicatesCacheManager> cm,
    std::shared_ptr<NaiveDynamicStringDictionary> extra_str_dict,
    TimeControl &time_control,
    std::shared_ptr<VarBindingQProc> var_binding_qproc,
    const std::string &temp_files_dir)
    : QueryIterator(time_control), input_it(std::move(input_it)),
      next_available(false), var_pos_mapping(get_var_pos_mapping(vim)),
      eval_data(vim, std::move(cm), var_pos_mapping, std::move(extra_str_dict),
                time_control, std::move(var_binding_qproc), temp_files_dir) {
  for (const auto *node : expr_nodes) {
    bool_expressions.push_back(
        ExprProcessor(eval_data, *node).create_evaluator());
  }

  next_concrete();
}
std::vector<unsigned long> FilterIterator::next_concrete() {
  if (!time_control.tick())
    return next_row;
  auto result = next_row;
  next_available = false;

  while (input_it->has_next()) {
    auto current = input_it->next();
    if (!time_control.tick())
      return result;
    bool accepted = false;
    for (const auto &bool_expr : bool_expressions) {
      accepted = bool_expr->eval_boolean(current);
      if (!accepted)
        break;
    }
    if (accepted) {
      next_row = std::move(current);
      next_available = true;
      return result;
    }
  }

  return result;
}

std::shared_ptr<std::unordered_map<std::string, unsigned long>>
FilterIterator::get_var_pos_mapping(VarIndexManager &vim) {
  auto result =
      std::make_shared<std::unordered_map<std::string, unsigned long>>();
  auto rev_map = vim.reverse();
  auto &headers = input_it->get_headers();
  for (unsigned long i = 0; i < static_cast<unsigned long>(headers.size());
       i++) {
    auto header = headers[i];
    (*result)[rev_map[header]] = i;
  }
  return result;
}