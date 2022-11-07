//
// Created by cristobal on 04-11-22.
//

#include "TPMSortedStreamer.hpp"

namespace k2cache {
proto_msg::CacheResponse TPMSortedStreamer::get_next_response() {
  return proto_msg::CacheResponse();
}
int TPMSortedStreamer::get_pattern_channel_id() { return 0; }
int TPMSortedStreamer::get_channel_id() { return 0; }
bool TPMSortedStreamer::all_sent() { return false; }

TPMSortedStreamer::TPMSortedStreamer(int channel_id, int pattern_channel_id,
                                     const TripleNodeId &triple_pattern_node,
                                     CacheContainer *cache,
                                     unsigned long threshold_part_size)
    : channel_id(channel_id), pattern_channel_id(pattern_channel_id),
      triple_pattern_node(triple_pattern_node), cache(cache),
      threshold_part_size(threshold_part_size) {
  initialize();
}
void TPMSortedStreamer::initialize() {
  auto &nis = cache->get_nodes_ids_manager();

  auto subject_variable = triple_pattern_node.subject.is_any();
  auto object_variable = triple_pattern_node.object.is_any();

  auto predicate_id = triple_pattern_node.predicate.get_value();
  auto predicate_id_translated = nis.get_id((long)predicate_id);

  auto fetch_result =
      cache->get_pcm().get_predicates_index_cache().fetch_k2tree(
          predicate_id_translated);
  std::unique_ptr<K2TreeScanner> k2tree_scanner;
  if (!fetch_result.exists()) {
    k2tree_scanner = cache->get_pcm().create_null_k2tree_scanner();
    return;
  }

  auto &k2tree = fetch_result.get_mutable();

  if (subject_variable && object_variable) {
    k2tree_scanner = k2tree.create_full_scanner();
  } else if (subject_variable) {
    auto object_id = triple_pattern_node.object.get_value();
    auto object_id_translated = nis.get_id((long)object_id);
    k2tree_scanner = k2tree.create_band_scanner(
        object_id_translated, K2TreeScanner::BandType::ROW_BAND_TYPE);
  } else if (object_variable) {
    auto subject_id = triple_pattern_node.subject.get_value();
    auto subject_id_translated = nis.get_id((long)subject_id);
    k2tree_scanner = k2tree.create_band_scanner(
        subject_id_translated, K2TreeScanner::BandType::COLUMN_BAND_TYPE);
  } else {
    // none variable
    k2tree_scanner = k2tree.create_empty_scanner();
  }

  get_all_data(*k2tree_scanner, subject_variable, object_variable);
}

TPMSortedStreamer::DataResults
TPMSortedStreamer::get_all_data(K2TreeScanner &scanner, bool subject_variable,
                                bool object_variable) {
  auto &nis = cache->get_nodes_ids_manager();
  std::vector<unsigned long> single_results;
  std::vector<std::pair<unsigned long, unsigned long>> pair_results;
  while (scanner.has_next()) {
    auto matching_pair_so = scanner.next();
    if (subject_variable && object_variable) {
      auto subject = nis.get_real_id((long)matching_pair_so.first);
      auto object = nis.get_real_id((long)matching_pair_so.second);
      pair_results.emplace_back(subject, object);
    } else if (subject_variable) {
      auto subject = nis.get_real_id((long)matching_pair_so.first);
      single_results.push_back(subject);
    } else { // else if (object_variable) {
      auto object = nis.get_real_id((long)matching_pair_so.second);
      single_results.push_back(object);
    }
  }
  DataResults results;
  results.single_results = std::move(single_results);
  results.pair_results = std::move(pair_results);
  return results;
}
} // namespace k2cache