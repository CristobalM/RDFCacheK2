//
// Created by cristobal on 7/21/21.
//

#include "SequenceProcessor.hpp"
#include "ResultTableIteratorCrossProduct.hpp"
#include "ResultTableIteratorEmpty.hpp"
#include "ResultTableIteratorJoinSeqBinding.hpp"
#include "VarBindingQProc.hpp"
#include <query_processing/utility/ProtoGatherVars.hpp>

std::shared_ptr<ResultTableIterator> SequenceProcessor::execute_it() {
  std::vector<std::shared_ptr<ResultTableIterator>> to_cross_product_iterators;
  for (size_t cc_i = 0; cc_i < cc_positions.size(); cc_i++) {
    auto &cc_pos = cc_positions[cc_i];
    auto &current_sets = cc_sets[cc_i];
    const auto &first_proto_node = sequence_node.nodes(cc_pos[0]);

    // std::set<unsigned long> resulting_vars_set_current;

    auto result_it =
        query_processor->process_node(first_proto_node, var_binding_qproc);

    // resulting_vars_set_current = current_sets[0];
    for (size_t i = 1; i < cc_pos.size(); i++) {
      int pos = cc_pos[i];
      const auto &curr_proto_node = sequence_node.nodes(pos);

      result_it = std::make_shared<ResultTableIteratorJoinSeqBinding>(
          result_it, curr_proto_node, var_binding_qproc, current_sets[i],
          time_control, query_processor->get_cache_manager(),
          query_processor->get_vim_ptr(),
          query_processor->get_extra_str_dict_ptr());
    }
    to_cross_product_iterators.push_back(std::move(result_it));
  }
  if (to_cross_product_iterators.empty()) {
    return std::make_shared<ResultTableIteratorEmpty>(resulting_headers,
                                                      time_control);
  }

  auto result_it = to_cross_product_iterators[0];
  for (size_t i = 1; i < to_cross_product_iterators.size(); i++) {
    auto curr_it = to_cross_product_iterators[i];
    result_it = std::make_shared<ResultTableIteratorCrossProduct>(
        result_it, std::move(curr_it), time_control);
  }
  return result_it;
}

SequenceProcessor::SequenceProcessor(
    const proto_msg::SequenceNode &sequence_node, QProc *query_processor,
    TimeControl &time_control,
    std::shared_ptr<VarBindingQProc> var_binding_qproc)
    : sequence_node(sequence_node), query_processor(query_processor),
      time_control(time_control),
      var_binding_qproc(std::move(var_binding_qproc)) {
  auto vd_graph = VarDependencyGraph(get_vars_sequence());
  auto [cc_positions, cc_sets] =
      vd_graph.get_connected_components_positions_with_sets();
  this->cc_positions = std::move(cc_positions);
  this->cc_sets = std::move(cc_sets);
  resulting_headers = generate_headers(vd_graph);
}

std::vector<VarsCollection> SequenceProcessor::get_vars_sequence() {
  std::vector<VarsCollection> result;
  for (int i = 0; i < sequence_node.nodes_size(); i++) {
    auto var_collection = get_vars_from_node(sequence_node.nodes(i), i);
    result.push_back(std::move(var_collection));
  }
  return result;
}

VarsCollection
SequenceProcessor::get_vars_from_node(const proto_msg::SparqlNode &node,
                                      int position) {
  auto gathered_vars =
      ProtoGatherVars::get_vars_from_node(query_processor->get_vim(), node);
  return VarsCollection(std::move(gathered_vars), position);
}

std::vector<unsigned long>
SequenceProcessor::generate_headers(VarDependencyGraph &vd_graph) {
  std::set<unsigned long> result_set;
  for (const auto &vdg_node : vd_graph.get_nodes()) {
    const auto &var_collection = vdg_node->get_collection();
    const auto &table_vars = var_collection.get_table_vars_c();
    for (auto var : table_vars) {
      result_set.insert(var);
    }
  }
  return std::vector<unsigned long>(result_set.begin(), result_set.end());
}
