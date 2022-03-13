//
// Created by cristobal on 3/2/22.
//

#include "StreamerFromCachedSource.hpp"
#include "CachedObjectScanner.hpp"
#include "CachedSubjectObjectScanner.hpp"
#include "CachedSubjectScanner.hpp"

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
    stream_response->set_has_exact_response(true);
    stream_response->set_exact_response(cached_source->has(
        triple_pattern_query.subject, triple_pattern_query.object));
    return cache_response;
  }

  stream_response->set_has_exact_response(false);

  while (cached_source_scanner->has_next()) {
    auto matching_pair_so = cached_source_scanner->next();
    auto *matching_values = stream_response->mutable_matching_values()->Add();
    if (subject_variable) {
      acc_size += sizeof(unsigned long);
      auto *s_match = matching_values->mutable_single_match()->Add();
      s_match->set_encoded_data((long)matching_pair_so.first);
    }
    if (object_variable) {
      acc_size += sizeof(unsigned long);
      auto *s_match = matching_values->mutable_single_match()->Add();
      s_match->set_encoded_data(matching_pair_so.second);
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
    int current_pattern_channel_id, TriplePatternQuery triple_pattern_query,
    Cache *cache, unsigned long threshold_part_size)
    : cached_source(cached_source), channel_id(channel_id),
      pattern_channel_id(current_pattern_channel_id),
      triple_pattern_query(triple_pattern_query), cache(cache),
      threshold_part_size(threshold_part_size), finished(false), first(true) {

  subject_variable = triple_pattern_query.subject == NODE_ANY;
  object_variable = triple_pattern_query.object == NODE_ANY;

  if (subject_variable && object_variable) {
    cached_source_scanner =
        std::make_unique<CachedSubjectObjectScanner>(cached_source);
  } else if (!subject_variable && !object_variable) {
    cached_source_scanner = nullptr;
  } else if (!subject_variable) { // only
    cached_source_scanner = std::make_unique<CachedObjectScanner>(
        cached_source, (unsigned long)triple_pattern_query.subject);
  } else { // !object_variable only
    cached_source_scanner = std::make_unique<CachedSubjectScanner>(
        cached_source, (unsigned long)triple_pattern_query.object);
  }
}

void StreamerFromCachedSource::set_finished() { finished = true; }
