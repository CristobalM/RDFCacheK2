//
// Created by cristobal on 7/11/21.
//

#include "BGPProcessor.hpp"
#include "nodeids/node_ids_constants.hpp"
#include <TimeControl.hpp>
#include <algorithm>
#include <query_processing/iterators/BGPIterator.hpp>
#include <query_processing/iterators/EmptyIterator.hpp>
#include <query_processing/iterators/bgpops/BGPOpsFactory.hpp>
namespace k2cache {

BGPProcessor::BGPProcessor(BgpMessage bgp_message,
                           PredicatesCacheManager &cm,
                           const NodeIdsManager &node_ids_manager,
                           VarIndexManager &vim, TimeControl &time_control
                           )
    : bgp_message(std::move(bgp_message)), cm(cm), node_ids_manager(node_ids_manager),
      vim(vim), time_control(time_control) {
  for(auto &triple : this->bgp_message.patterns){
    triple.subject.cache_node_id = get_cache_id(triple.subject.real_node_id);
    triple.predicate.cache_node_id = get_cache_id(triple.predicate.real_node_id);
    triple.object.cache_node_id = get_cache_id(triple.object.real_node_id);
  }
}


std::shared_ptr<QueryIterator> BGPProcessor::execute_it() {
  find_headers();
  if (!do_all_predicates_have_trees()) {
    return std::make_shared<EmptyIterator>(std::move(header_vec));
  }
  auto scanners = build_scanners();
  auto ops = build_bgp_ops(std::move(scanners));
  return std::make_shared<BGPIterator>(std::move(ops), header_vec,
                                       time_control);
}
void BGPProcessor::find_headers() {
  for (const auto &triple : bgp_message.patterns) {

    if (!triple.subject.is_concrete) {
      vim.assign_index_if_not_found(triple.subject.var_name);
    }
    if (!triple.object.is_concrete) {
      vim.assign_index_if_not_found(triple.object.var_name);
    }
    auto subject_is_var = !triple.subject.is_concrete;
    auto object_is_var = !triple.object.is_concrete;
    if (subject_is_var) {
      add_variable(triple.subject);
    }
    if (object_is_var) {
      add_variable(triple.object);
    }
  }
  header_vec =
      std::vector<unsigned long>(header_values.begin(), header_values.end());
  join_incidence = std::vector<long>(header_vec.size(), 0);
  build_rev_map();
}

void BGPProcessor::add_variable(const BgpNode &node) {
  auto var_id = vim.assign_index_if_not_found(node.var_name);
  if (var_id != 0){
    header_values.insert(var_id);
  }
}
std::vector<std::unique_ptr<K2TreeScanner>> BGPProcessor::build_scanners() {
  std::vector<std::unique_ptr<K2TreeScanner>> scanners;

  scanners.reserve(bgp_message.patterns.size());
  for (const auto &triple : bgp_message.patterns) {
    std::unique_ptr<K2TreeScanner> scanner;

    auto fetched = cm.get_predicates_index_cache()
                       .fetch_k2tree(triple.predicate.cache_node_id);
    auto &tree = fetched.get_mutable();



    if (!triple.subject.is_concrete) { // is var
      vim.assign_index_if_not_found(triple.subject.var_name);
    }
    if (!triple.object.is_concrete) { // is var
      vim.assign_index_if_not_found(triple.object.var_name);
    }

    auto subject_is_var = !triple.subject.is_concrete;
    auto object_is_var = !triple.object.is_concrete;

    if (subject_is_var && object_is_var) {
      scanner = tree.create_full_scanner();
    } else if (subject_is_var) {
      scanner = tree.create_band_scanner(
          triple.object.cache_node_id, K2TreeScanner::BandType::ROW_BAND_TYPE);
    } else if (object_is_var) {
      scanner = tree.create_band_scanner(
          triple.subject.cache_node_id, K2TreeScanner::BandType::COLUMN_BAND_TYPE);
    } else {
      scanner = tree.create_empty_scanner();
    }
    scanners.push_back(std::move(scanner));
  }
  return scanners;
}

std::vector<std::unique_ptr<BGPOp>> BGPProcessor::build_bgp_ops(
    std::vector<std::unique_ptr<K2TreeScanner>> &&scanners) {
  std::vector<std::unique_ptr<BGPOp>> ops;

  for (size_t i = 0; i < scanners.size(); i++) {
    ops.push_back(BGPOpsFactory::create_bgp_op(
        std::move(scanners[i]), bgp_message.patterns[i], vim, join_incidence,
        header_reverse_map, time_control));
  }

  return ops;
}
void BGPProcessor::build_rev_map() {
  header_reverse_map.clear();
  for (size_t i = 0; i < header_vec.size(); i++) {
    header_reverse_map[header_vec[i]] = i;
  }
}
bool BGPProcessor::do_all_predicates_have_trees() {
  return std::all_of(bgp_message.patterns.begin(), bgp_message.patterns.end(), [&](const BgpTriple &triple) {
    return triple.predicate.cache_node_id != 0 &&
           cm.get_predicates_index_cache()
               .fetch_k2tree(triple.predicate.cache_node_id)
               .exists();
  });
}
uint64_t BGPProcessor::get_cache_id(uint64_t real_id) const {
  auto value = node_ids_manager.get_id(real_id);
  if(value == NOT_FOUND_NODEID){
    return -1;
  }
  return value;
}
std::vector<unsigned long> BGPProcessor::get_permutation_vec() {
  std::vector<unsigned long> permutation_vec;
  permutation_vec.reserve(bgp_message.var_names.size());
  for(const auto &var_name : bgp_message.var_names){
    auto internal_index = vim.var_indexes[var_name];
    auto pos_result = header_reverse_map[internal_index];
    permutation_vec.push_back(pos_result);
  }
  return permutation_vec;
}

}
