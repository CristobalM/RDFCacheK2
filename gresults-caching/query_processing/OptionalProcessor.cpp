//
// Created by cristobal on 25-07-21.
//

#include "OptionalProcessor.hpp"
#include "ResultTableIteratorLeftOuterJoin.hpp"
#include <memory>
#include <query_processing/utility/ProtoGatherVars.hpp>
OptionalProcessor::OptionalProcessor(
    const proto_msg::SparqlNode &left_node,
    const proto_msg::SparqlNode &right_node, QProc *query_processor,
    std::shared_ptr<VarBindingQProc> var_binding_qproc,
    TimeControl &time_control)
    : left_node(left_node), right_node(right_node),
      query_processor(query_processor),
      var_binding_qproc(std::move(var_binding_qproc)),
      time_control(time_control) {}
std::shared_ptr<ResultTableIterator> OptionalProcessor::execute_it() {
  auto left_it = query_processor->process_node(left_node, var_binding_qproc);
  auto right_table_vars = get_right_table_vars_set();
  return std::make_shared<ResultTableIteratorLeftOuterJoin>(
      std::move(left_it), right_node, var_binding_qproc, right_table_vars,
      time_control, query_processor->get_cache_manager(),
      query_processor->get_vim_ptr(),
      query_processor->get_extra_str_dict_ptr());
}
std::set<unsigned long> OptionalProcessor::get_right_table_vars_set() {
  auto gathered_vars = ProtoGatherVars::get_vars_from_node(
      query_processor->get_vim(), right_node);
  return gathered_vars.table_vars;
}
