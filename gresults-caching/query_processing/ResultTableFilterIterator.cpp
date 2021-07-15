//
// Created by cristobal on 7/14/21.
//

#include "ResultTableFilterIterator.hpp"
#include "ExprProcessor.hpp"
bool ResultTableFilterIterator::has_next() { return next_available; }
std::vector<unsigned long> ResultTableFilterIterator::next() {
  return next_concrete();
}
std::vector<unsigned long> &ResultTableFilterIterator::get_headers() {
  return input_it->get_headers();
}
void ResultTableFilterIterator::reset_iterator() {}
ResultTableFilterIterator::ResultTableFilterIterator(
    std::shared_ptr<ResultTableIterator> input_it, VarIndexManager &vim,
    std::vector<const proto_msg::ExprNode *> &expr_nodes,
    const PredicatesCacheManager &cm,
    NaiveDynamicStringDictionary &extra_str_dict)
    : input_it(std::move(input_it)), var_pos_mapping(get_var_pos_mapping(vim)),
      eval_data(vim, cm, var_pos_mapping, extra_str_dict) {
  for (const auto *node : expr_nodes) {
    bool_expressions.push_back(
        ExprProcessor(eval_data, *node).create_evaluator());
  }

  next_concrete();
}
std::vector<unsigned long> ResultTableFilterIterator::next_concrete() {
  auto result = next_row;
  next_available = false;

  while (input_it->has_next()) {
    auto current = input_it->next();
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
ResultTableFilterIterator::get_var_pos_mapping(VarIndexManager &vim) {
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