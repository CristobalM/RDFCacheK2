#include "ExprListProcessor.hpp"
#include "ExprProcessor.hpp"
#include "expr/EvalData.hpp"

ExprListProcessor::ExprListProcessor(
    ResultTable &table, VarIndexManager &vim,
    const std::vector<const proto_msg::ExprNode *> &expr_list,
    const PredicatesCacheManager &cm,
    NaiveDynamicStringDictionary &extra_str_dict)
    : table(table), vim(vim), expr_list(expr_list), cm(cm),
      extra_str_dict(extra_str_dict) {}

std::shared_ptr<std::unordered_map<std::string, unsigned long>>
ExprListProcessor::get_var_pos_mapping() {
  auto result =
      std::make_shared<std::unordered_map<std::string, unsigned long>>();
  auto rev_map = vim.reverse();

  for (unsigned long i = 0;
       i < static_cast<unsigned long>(table.headers.size()); i++) {
    auto header = table.headers[i];
    (*result)[rev_map[header]] = i;
  }
  return result;
}

void ExprListProcessor::execute() {
  auto &data = table.data;

  auto var_pos_mapping = get_var_pos_mapping();

  std::vector<std::unique_ptr<ExprEval>> bool_expressions;

  EvalData eval_data(vim, cm, var_pos_mapping, extra_str_dict);
  for (const auto *node : expr_list) {
    bool_expressions.push_back(
        ExprProcessor(eval_data, *node).create_evaluator());
  }

  for (auto it = data.begin(); it != data.end();) {
    auto next = std::next(it);
    bool accepted = true;
    for (const auto &bool_expr : bool_expressions) {
      accepted = bool_expr->eval_boolean(*it);
      if (!accepted)
        break;
    }
    if (!accepted) {
      data.erase(it);
    }
    it = next;
  }
}
