//
// Created by cristobal on 04-11-22.
//

#include "TPMSortedStreamer.hpp"

namespace k2cache {
proto_msg::CacheResponse TPMSortedStreamer::get_next_response() {
  if (!data_results.pair_results.empty())
    return get_next_pairs_response();
  if (!data_results.single_results.empty())
    return get_next_single_response();
  return get_empty_response();
}
int TPMSortedStreamer::get_pattern_channel_id() { return pattern_channel_id; }
int TPMSortedStreamer::get_channel_id() { return channel_id; }
bool TPMSortedStreamer::all_sent() {
  return (!data_results.pair_results.empty() &&
          current_sent >= data_results.pair_results.size()) ||
         (!data_results.single_results.empty() &&
          current_sent >= data_results.single_results.size());
}

TPMSortedStreamer::TPMSortedStreamer(int channel_id, int pattern_channel_id,
                                     const TripleNodeId &triple_pattern_node,
                                     CacheContainer *cache,
                                     unsigned long threshold_part_size)
    : channel_id(channel_id), pattern_channel_id(pattern_channel_id),
      triple_pattern_node(triple_pattern_node), cache(cache),
      threshold_part_size(threshold_part_size), current_sent(0) {
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
  sort_data();
}

void TPMSortedStreamer::get_all_data(K2TreeScanner &scanner,
                                     bool subject_variable,
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
  data_results.single_results = std::move(single_results);
  data_results.pair_results = std::move(pair_results);
}
void TPMSortedStreamer::sort_data() {
  std::sort(data_results.single_results.begin(),
            data_results.single_results.end());
  std::sort(data_results.pair_results.begin(), data_results.pair_results.end());
}
proto_msg::CacheResponse TPMSortedStreamer::get_next_pairs_response() {
  proto_msg::CacheResponse cache_response;
  cache_response.set_response_type(
      proto_msg::MessageType::STREAM_OF_TRIPLES_MATCHING_PATTERN_RESPONSE);
  auto *stream_response =
      cache_response.mutable_stream_of_triples_matching_pattern_response();

  stream_response->set_channel_id(channel_id);
  stream_response->set_pattern_channel_id(pattern_channel_id);

  auto acc_size = 0UL;

  unsigned long i = current_sent;
  for (; i < data_results.pair_results.size() && acc_size < threshold_part_size;
       i++) {
    acc_size += sizeof(unsigned long) * 2;

    auto *matching_values = stream_response->mutable_matching_values()->Add();
    auto *first_match = matching_values->mutable_single_match()->Add();
    first_match->set_encoded_data(data_results.pair_results[i].first);
    auto *second_match = matching_values->mutable_single_match()->Add();
    second_match->set_encoded_data(data_results.pair_results[i].second);
  }

  if (i >= data_results.pair_results.size()) {
    stream_response->set_last_result(true);
  } else {
    stream_response->set_last_result(false);
  }

  return cache_response;
}

proto_msg::CacheResponse TPMSortedStreamer::get_next_single_response() {
  proto_msg::CacheResponse cache_response;
  cache_response.set_response_type(
      proto_msg::MessageType::STREAM_OF_TRIPLES_MATCHING_PATTERN_RESPONSE);
  auto *stream_response =
      cache_response.mutable_stream_of_triples_matching_pattern_response();

  stream_response->set_channel_id(channel_id);
  stream_response->set_pattern_channel_id(pattern_channel_id);

  auto acc_size = 0UL;

  unsigned long i = current_sent;
  for (;
       i < data_results.single_results.size() && acc_size < threshold_part_size;
       i++) {
    acc_size += sizeof(unsigned long);

    auto *matching_values = stream_response->mutable_matching_values()->Add();
    auto *s_match = matching_values->mutable_single_match()->Add();
    s_match->set_encoded_data(data_results.single_results[i]);
  }

  if (i >= data_results.single_results.size()) {
    stream_response->set_last_result(true);
  } else {
    stream_response->set_last_result(false);
  }

  return cache_response;
}

proto_msg::CacheResponse TPMSortedStreamer::get_empty_response() {
  proto_msg::CacheResponse cache_response;
  cache_response.set_response_type(
      proto_msg::MessageType::STREAM_OF_TRIPLES_MATCHING_PATTERN_RESPONSE);
  auto *stream_response =
      cache_response.mutable_stream_of_triples_matching_pattern_response();

  stream_response->set_last_result(true);
  stream_response->set_has_exact_response(true);
  stream_response->set_channel_id(channel_id);
  stream_response->set_pattern_channel_id(pattern_channel_id);

  auto predicate_id = triple_pattern_node.predicate.get_value();
  auto &nis = cache->get_nodes_ids_manager();

  auto predicate_id_translated = nis.get_id((long)predicate_id);

  auto fetch_result =
      cache->get_pcm().get_predicates_index_cache().fetch_k2tree(
          predicate_id_translated);
  auto &k2tree = fetch_result.get_mutable();

  auto subject_id = triple_pattern_node.subject.get_value();
  auto object_id = triple_pattern_node.object.get_value();

  stream_response->set_exact_response(k2tree.has(subject_id, object_id));
  return cache_response;
}

} // namespace k2cache