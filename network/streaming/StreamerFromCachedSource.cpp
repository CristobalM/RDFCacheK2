//
// Created by cristobal on 3/2/22.
//

#include "StreamerFromCachedSource.hpp"
#include "scanner/CachedObjectScanner.hpp"
#include "scanner/CachedSubjectObjectScanner.hpp"
#include "scanner/CachedSubjectScanner.hpp"

#include <utility>
namespace k2cache {
proto_msg::CacheResponse StreamerFromCachedSource::get_next_response() {
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
    auto subject_id = triple_pattern_node.subject().encoded_data();
    auto object_id = triple_pattern_node.object().encoded_data();
    stream_response->set_has_exact_response(true);
    stream_response->set_exact_response(
        cached_source->has(subject_id, object_id));
    return cache_response;
  }

  stream_response->set_has_exact_response(false);

  auto &nis = cache->get_nodes_ids_manager();

  while (cached_source_scanner->has_next()) {
    auto matching_pair_so = cached_source_scanner->next();
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

  if (!cached_source_scanner->has_next()) {
    stream_response->set_last_result(true);
    set_finished();
  } else {
    stream_response->set_last_result(false);
  }

  return cache_response;
}

int StreamerFromCachedSource::get_pattern_channel_id() {
  return pattern_channel_id;
}
int StreamerFromCachedSource::get_channel_id() { return channel_id; }
bool StreamerFromCachedSource::all_sent() { return finished; }
StreamerFromCachedSource::StreamerFromCachedSource(
    I_CachedPredicateSource *cached_source, int channel_id,
    int current_pattern_channel_id,
    proto_msg::TripleNodeIdEnc triple_pattern_node, CacheContainer *cache,
    unsigned long threshold_part_size)
    : cached_source(cached_source), channel_id(channel_id),
      pattern_channel_id(current_pattern_channel_id),
      triple_pattern_node(std::move(triple_pattern_node)), cache(cache),
      threshold_part_size(threshold_part_size), finished(false), first(true) {

  subject_variable =
      (long)this->triple_pattern_node.subject().encoded_data() == NODE_ANY;
  object_variable =
      (long)this->triple_pattern_node.object().encoded_data() == NODE_ANY;

  translated_subject = -1;
  translated_object = -1;

  auto &nis = cache->get_nodes_ids_manager();

  if (!subject_variable)
    translated_subject =
        nis.get_id((long)this->triple_pattern_node.subject().encoded_data());

  if (!object_variable)
    translated_object =
        nis.get_id((long)this->triple_pattern_node.object().encoded_data());

  if (subject_variable && object_variable) {
    cached_source_scanner =
        std::make_unique<CachedSubjectObjectScanner>(cached_source);
  } else if (!subject_variable && !object_variable) {
    cached_source_scanner = nullptr;
  } else if (!subject_variable) { // only
    cached_source_scanner = std::make_unique<CachedObjectScanner>(
        cached_source, translated_subject);
  } else { // !object_variable only
    cached_source_scanner = std::make_unique<CachedSubjectScanner>(
        cached_source, translated_object);
  }
}
void StreamerFromCachedSource::set_finished() { finished = true; }
} // namespace k2cache