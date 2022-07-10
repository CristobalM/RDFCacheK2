//
// Created by cristobal on 9/1/21.
//

#include "TriplePatternMatchingStreamer.hpp"

namespace k2cache {
TriplePatternMatchingStreamer::TriplePatternMatchingStreamer(
    int channel_id, int pattern_channel_id,
    const TripleNodeId &triple_pattern_node, CacheContainer *cache,
    unsigned long threshold_part_size)
    : channel_id(channel_id), pattern_channel_id(pattern_channel_id),
      triple_pattern_node(triple_pattern_node), cache(cache),
      threshold_part_size(threshold_part_size), first(true), finished(false) {
  initialize_scanner();
}

proto_msg::CacheResponse TriplePatternMatchingStreamer::get_next_response() {
  proto_msg::CacheResponse cache_response;
  cache_response.set_response_type(
      proto_msg::MessageType::STREAM_OF_TRIPLES_MATCHING_PATTERN_RESPONSE);
  auto *stream_response =
      cache_response.mutable_stream_of_triples_matching_pattern_response();
  unsigned long acc_size = 0;
  if (first) {
    first = false;
  }

  stream_response->set_channel_id(channel_id);
  stream_response->set_pattern_channel_id(pattern_channel_id);

  // exact match request
  if (!subject_variable && !object_variable) {
    stream_response->set_last_result(true);
    set_finished();
    auto subject_id = triple_pattern_node.subject.get_value();
    auto object_id = triple_pattern_node.object.get_value();
    stream_response->set_has_exact_response(true);
    stream_response->set_exact_response(
        k2tree_scanner->get_tree().has(subject_id, object_id));
    return cache_response;
  }

  stream_response->set_has_exact_response(false);

  auto &nis = cache->get_nodes_ids_manager();

  while (k2tree_scanner->has_next()) {
    auto matching_pair_so = k2tree_scanner->next();
    auto *matching_values = stream_response->mutable_matching_values()->Add();
    if (subject_variable) {
      acc_size += sizeof(unsigned long);
      auto *s_match = matching_values->mutable_single_match()->Add();
      auto original_value = nis.get_real_id((long)matching_pair_so.first);
      s_match->set_encoded_data(original_value);
    }
    if (object_variable) {
      acc_size += sizeof(unsigned long);
      auto *s_match = matching_values->mutable_single_match()->Add();
      auto original_value = nis.get_real_id((long)matching_pair_so.second);
      s_match->set_encoded_data(original_value);
    }

    if (acc_size > threshold_part_size) {
      break;
    }
  }

  if (!k2tree_scanner->has_next()) {
    stream_response->set_last_result(true);
    set_finished();
  } else {
    stream_response->set_last_result(false);
  }

  return cache_response;
}
int TriplePatternMatchingStreamer::get_pattern_channel_id() {
  return pattern_channel_id;
}
int TriplePatternMatchingStreamer::get_channel_id() { return channel_id; }
bool TriplePatternMatchingStreamer::all_sent() { return finished; }

void TriplePatternMatchingStreamer::initialize_scanner() {
  auto &nis = cache->get_nodes_ids_manager();

  subject_variable = (long)triple_pattern_node.subject.is_any();
  object_variable = (long)triple_pattern_node.object.is_any();

  auto predicate_id = triple_pattern_node.predicate.get_value();
  auto predicate_id_translated = nis.get_id((long)predicate_id);

  auto fetch_result =
      cache->get_pcm().get_predicates_index_cache().fetch_k2tree(
          predicate_id_translated);
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
}

void TriplePatternMatchingStreamer::set_finished() { finished = true; }
proto_msg::CacheResponse
TriplePatternMatchingStreamer::timeout_proto_response() {
  set_finished();
  proto_msg::CacheResponse result;
  result.set_response_type(proto_msg::TIMED_OUT_RESPONSE);
  result.mutable_error_response();
  return result;
}
} // namespace k2cache
