//
// Created by cristobal on 5/18/21.
//

#include "ExistsEval.hpp"
#include "ExprEval.hpp"
#include <query_processing/QueryProcessor.hpp>
#include <query_processing/utility/ProtoGatherVars.hpp>
void ExistsEval::init() {
  ExprEval::init();
  was_calculated_constant = false;
  is_constant = false;
}
std::shared_ptr<TermResource>
ExistsEval::eval_resource(const ExprEval::row_t &row) {
  return generate_from_eval_boolean(row);
}
bool ExistsEval::eval_boolean(const ExprEval::row_t &row) {
  has_constant_subtree();
  const auto &query_tree = expr_node.function_node().exprs(0).pattern_node();
  if (!is_constant) {
    auto next_vim = std::make_shared<VarIndexManager>(eval_data.vim);
    auto var_binding_qproc =
        bind_row_vars_next_eval_data(*eval_data.extra_dict, row);
    auto qproc =
        QueryProcessor(eval_data.cm, std::move(next_vim), eval_data.extra_dict,
                       eval_data.time_control, eval_data.temp_files_dir);
    auto rit = qproc.run_query(query_tree, std::move(var_binding_qproc));
    return rit.get_it().has_next();
  } else {
    auto rit = QueryProcessor(eval_data.cm, eval_data.time_control,
                              eval_data.temp_files_dir)
                   .run_query(query_tree);
    return rit.get_it().has_next();
  }
}
void ExistsEval::validate() {
  ExprEval::validate();
  assert_fun_size(1);
  assert_is_pattern_node(expr_node.function_node().exprs(0));
}
bool ExistsEval::has_constant_subtree() {
  if (was_calculated_constant)
    return is_constant;
  auto inside_vars =
      ProtoGatherVars::get_vars_from_node(
          eval_data.vim, expr_node.function_node().exprs(0).pattern_node())
          .all_vars;
  auto outside_vars = gather_outside_variables();

  std::set<unsigned long> intersection;
  std::set_intersection(inside_vars.begin(), inside_vars.end(),
                        outside_vars.begin(), outside_vars.end(),
                        std::inserter(intersection, intersection.begin()));
  is_constant = intersection.empty();
  was_calculated_constant = true;
  return is_constant;
}

std::set<unsigned long> ExistsEval::gather_outside_variables() const {
  std::set<unsigned long> vars;
  for (const auto &pair_var_value : eval_data.vim.var_indexes) {
    vars.insert(eval_data.vim.assign_index_if_not_found(pair_var_value.first));
  }
  return vars;
}

std::shared_ptr<VarBindingQProc> ExistsEval::bind_row_vars_next_eval_data(
    NaiveDynamicStringDictionary &dictionary, const row_t &row) {
  auto result = std::make_shared<VarBindingQProc>();
  for (const auto &var_pair : eval_data.vim.var_indexes) {
    const auto &var_name = var_pair.first;
    const auto var_id = var_pair.second;
    RDFResource resource;
    if (this->eval_data.var_pos_mapping->find(var_name) ==
        this->eval_data.var_pos_mapping->end())
      continue;
    auto pos = this->eval_data.var_pos_mapping->at(var_name);
    auto value_id = row[pos];
    auto last_cache_id = this->eval_data.cm->get_last_id();
    if (value_id > last_cache_id)
      resource = this->eval_data.extra_dict->extract_resource(value_id -
                                                              last_cache_id);
    else
      resource = this->eval_data.cm->extract_resource(value_id);

    dictionary.add_resource(resource);
    result->bind(var_id, value_id);
  }
  return result;
}
