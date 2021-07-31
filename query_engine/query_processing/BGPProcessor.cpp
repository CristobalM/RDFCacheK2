//
// Created by cristobal on 7/11/21.
//

#include "BGPProcessor.hpp"
#include "BGPOpsFactory.hpp"
#include "ResultTableIteratorEmpty.hpp"
#include "VarBindingQProc.hpp"
#include <TimeControl.hpp>
BGPProcessor::BGPProcessor(const proto_msg::BGPNode &bgp_node,
                           const PredicatesCacheManager &cm,
                           VarIndexManager &vim, TimeControl &time_control,
                           std::shared_ptr<VarBindingQProc> var_binding_qproc)
    : bgp_node(bgp_node), cm(cm), vim(vim), time_control(time_control),
      var_binding_qproc(std::move(var_binding_qproc)) {
  set_triples_from_proto();
}

void BGPProcessor::set_triples_from_proto() {
  for (int i = 0; i < bgp_node.triple_size(); i++) {
    const auto &triple = bgp_node.triple()[i];
    triples.push_back(std::make_shared<Triple>(triple, cm));
  }
}

std::shared_ptr<ResultTableIterator> BGPProcessor::execute_it() {
  find_headers();
  if (!do_all_predicates_have_trees()) {
    return std::make_shared<ResultTableIteratorEmpty>(std::move(header_vec),
                                                      time_control);
  }
  auto scanners = build_scanners();
  auto ops = build_bgp_ops(std::move(scanners));
  return std::make_shared<ResultTableIteratorBGP>(std::move(ops), header_vec,
                                                  time_control);
}
void BGPProcessor::find_headers() {
  for (auto &triple : triples) {

    unsigned long subject_id = 0;
    unsigned long object_id = 0;

    if (triple->subject.type == VAR) {
      subject_id = vim.assign_index_if_not_found(triple->subject.value);
    }
    if (triple->object.type == VAR) {
      object_id = vim.assign_index_if_not_found(triple->object.value);
    }
    auto subject_is_var = triple->subject.type == TermType::VAR &&
                          !var_binding_qproc->is_bound(subject_id);
    auto object_is_var = triple->object.type == TermType::VAR &&
                         !var_binding_qproc->is_bound(object_id);
    if (subject_is_var) {
      add_variable(triple->subject);
    }
    if (object_is_var) {
      add_variable(triple->object);
    }
  }
  header_vec =
      std::vector<unsigned long>(header_values.begin(), header_values.end());
  join_incidence = std::vector<long>(header_vec.size(), 0);
  build_rev_map();
}
void BGPProcessor::add_variable(Term &term) {
  auto var_id = vim.assign_index_if_not_found(term.value);
  if (var_id != 0 && !var_binding_qproc->is_bound(var_id))
    header_values.insert(var_id);
}
std::vector<std::unique_ptr<K2TreeScanner>> BGPProcessor::build_scanners() {
  std::vector<std::unique_ptr<K2TreeScanner>> scanners;

  scanners.reserve(triples.size());
  for (auto &triple : triples) {
    std::unique_ptr<K2TreeScanner> scanner;

    auto fetched = cm.get_tree_by_predicate_index(triple->predicate.id_value);
    auto &tree = fetched.get_mutable();

    unsigned long subject_id = 0;
    unsigned long object_id = 0;

    if (triple->subject.type == VAR) {
      subject_id = vim.assign_index_if_not_found(triple->subject.value);
    }
    if (triple->object.type == VAR) {
      object_id = vim.assign_index_if_not_found(triple->object.value);
    }

    auto subject_is_var = triple->subject.type == TermType::VAR &&
                          !var_binding_qproc->is_bound(subject_id);
    auto object_is_var = triple->object.type == TermType::VAR &&
                         !var_binding_qproc->is_bound(object_id);

    if (subject_is_var && object_is_var) {
      scanner = tree.create_full_scanner();
    } else if (subject_is_var) {
      unsigned long band_value;
      if (triple->object.type == TermType::VAR) {
        band_value = var_binding_qproc->get_value(object_id);
      } else {
        band_value = triple->object.id_value;
      }
      scanner = tree.create_band_scanner(
          band_value, K2TreeScanner::BandType::ROW_BAND_TYPE);
    } else if (object_is_var) {
      unsigned long band_value;
      if (triple->subject.type == TermType::VAR) {
        band_value = var_binding_qproc->get_value(subject_id);
      } else {
        band_value = triple->subject.id_value;
      }
      scanner = tree.create_band_scanner(
          band_value, K2TreeScanner::BandType::COLUMN_BAND_TYPE);
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
        std::move(scanners[i]), *triples[i], vim, join_incidence,
        header_reverse_map, time_control, *var_binding_qproc));
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
  return std::all_of(triples.begin(), triples.end(), [&](auto &triple) {
    return triple->predicate.id_value != 0 &&
           cm.get_tree_by_predicate_index(triple->predicate.id_value).exists();
  });
}
