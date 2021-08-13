//
// Created by cristobal on 25-07-21.
//

#include "MinusProcessor.hpp"
#include <query_processing/iterators/EmptyIterator.hpp>
#include <query_processing/iterators/MinusIterator.hpp>
#include <query_processing/utility/ProtoGatherVars.hpp>
MinusProcessor::MinusProcessor(
    const proto_msg::MinusNode &minus_node, QProc *query_processor,
    TimeControl &time_control,
    std::shared_ptr<VarBindingQProc> var_binding_qproc)
    : minus_node(minus_node), query_processor(query_processor),
      time_control(time_control),
      var_binding_qproc(std::move(var_binding_qproc)) {}
std::shared_ptr<QueryIterator> MinusProcessor::execute_it() {
  auto left_it =
      query_processor->process_node(minus_node.left_node(), var_binding_qproc);
  if (!time_control.tick())
    return std::make_shared<EmptyIterator>(time_control);
  auto right_vars_set = get_right_vars_set();
  if (!with_shared_headers(left_it->get_headers(), right_vars_set))
    return left_it;
  return std::make_shared<MinusIterator>(
      left_it, minus_node.right_node(), var_binding_qproc, time_control,
      query_processor->get_cache_manager(), query_processor->get_vim_ptr(),
      query_processor->get_extra_str_dict_ptr(),
      query_processor->get_temp_files_dir());
}

bool MinusProcessor::with_shared_headers(
    const std::vector<unsigned long> &left_headers,
    std::set<unsigned long> &right_vars_set) {
  auto left_vars =
      std::set<unsigned long>(left_headers.begin(), left_headers.end());
  std::set<unsigned long> intersection;
  std::set_intersection(left_vars.begin(), left_vars.end(),
                        right_vars_set.begin(), right_vars_set.end(),
                        std::inserter(intersection, intersection.begin()));
  return !intersection.empty();
}
std::set<unsigned long> MinusProcessor::get_right_vars_set() {
  return ProtoGatherVars::get_vars_from_node(query_processor->get_vim(),
                                             minus_node.right_node())
      .all_vars;
}
