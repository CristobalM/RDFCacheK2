//
// Created by cristobal on 7/11/21.
//

#include "BGPProcessor.hpp"
#include "BGPOpsFactory.hpp"
#include "ResultTableIteratorEmpty.hpp"
#include <TimeControl.hpp>
BGPProcessor::BGPProcessor(const proto_msg::BGPNode &bgp_node,
                           const PredicatesCacheManager &cm,
                           VarIndexManager &vim, TimeControl &time_control)
    : bgp_node(bgp_node), cm(cm), vim(vim), time_control(time_control) {
  set_triples_from_proto();
}

void BGPProcessor::set_triples_from_proto() {
  for (int i = 0; i < bgp_node.triple_size(); i++) {
    const auto &triple = bgp_node.triple()[i];
    triples.push_back(std::make_shared<Triple>(triple, cm));
  }
}

std::shared_ptr<ResultTable> BGPProcessor::execute() {
  auto it_handler = execute_it();
  auto result = std::make_shared<ResultTable>();

  result->headers = it_handler->get_headers();
  while (it_handler->has_next()) {
    result->data.push_back(it_handler->next());
  }
  return result;
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
    if (triple->subject.type == TermType::VAR) {
      add_variable(triple->subject);
    }
    if (triple->object.type == TermType::VAR) {
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
  if (var_id != 0)
    header_values.insert(var_id);
}
std::vector<std::unique_ptr<K2TreeMixed::K2TreeScanner>>
BGPProcessor::build_scanners() {
  std::vector<std::unique_ptr<K2TreeMixed::K2TreeScanner>> scanners;

  for (auto &triple : triples) {
    std::unique_ptr<K2TreeMixed::K2TreeScanner> scanner;

    auto fetched = cm.get_tree_by_predicate_index(triple->predicate.id_value);
    auto &tree = fetched.get_mutable();

    if (triple->subject.type == TermType::VAR &&
        triple->object.type == TermType::VAR) {
      scanner = tree.create_full_scanner(time_control);
    } else if (triple->subject.type == TermType::VAR) {
      scanner = tree.create_band_scanner(triple->object.id_value,
                                         K2TreeMixed::BandType::ROW_BAND_TYPE,
                                         time_control);
    } else if (triple->object.type == TermType::VAR) {
      scanner = tree.create_band_scanner(
          triple->subject.id_value, K2TreeMixed::BandType::COLUMN_BAND_TYPE,
          time_control);
    } else {
      scanner = tree.create_empty_scanner();
    }
    scanners.push_back(std::move(scanner));
  }
  return scanners;
}

std::vector<std::unique_ptr<BGPOp>> BGPProcessor::build_bgp_ops(
    std::vector<std::unique_ptr<K2TreeMixed::K2TreeScanner>> &&scanners) {
  std::vector<std::unique_ptr<BGPOp>> ops;

  for (size_t i = 0; i < scanners.size(); i++) {
    ops.push_back(BGPOpsFactory::create_bgp_op(
        std::move(scanners[i]), triples[i], vim, join_incidence,
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
  return std::all_of(triples.begin(), triples.end(), [&](auto &triple) {
    return triple->predicate.id_value != 0 &&
           cm.get_tree_by_predicate_index(triple->predicate.id_value).exists();
  });
}
