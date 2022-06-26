//
// Created by cristobal on 8/31/21.
//

#include "TripleMatchesPartStreamer.hpp"

#include "StreamerFromCachedSource.hpp"
#include "TriplePatternMatchingStreamer.hpp"
#include "fic/FullyIndexedPredicate.hpp"
#include <map>
namespace k2cache {
TripleMatchesPartStreamer::TripleMatchesPartStreamer(
    int channel_id, std::vector<unsigned long> &&loaded_predicates,
    size_t threshold_part_size, TaskProcessor *task_processor,
    CacheContainer *cache, FullyIndexedCache &fully_indexed_cache)
    : channel_id(channel_id), loaded_predicates(std::move(loaded_predicates)),
      threshold_part_size(threshold_part_size), cache(cache),
      task_processor(task_processor), current_pattern_channel_id(0),
      done(false), fully_indexed_cache(fully_indexed_cache) {
  init_cached_predicate_sources();
}

int TripleMatchesPartStreamer::get_id() { return channel_id; }
proto_msg::CacheResponse TripleMatchesPartStreamer::timeout_proto() {
  set_finished();
  proto_msg::CacheResponse result;
  result.set_response_type(proto_msg::TIMED_OUT_RESPONSE);
  result.mutable_error_response();
  return result;
}

void TripleMatchesPartStreamer::set_finished() { done = true; }
proto_msg::CacheResponse
TripleMatchesPartStreamer::time_control_finished_error() {
  set_finished();
  proto_msg::CacheResponse cache_response;
  cache_response.set_response_type(
      proto_msg::MessageType::INVALID_QUERY_RESPONSE);
  cache_response.mutable_invalid_query_response();
  return cache_response;
}
const std::vector<unsigned long> &
TripleMatchesPartStreamer::get_predicates_in_use() {
  return loaded_predicates;
}

TripleMatchesPartStreamer::~TripleMatchesPartStreamer() {
  if (!loaded_predicates.empty())
    task_processor->mark_ready(loaded_predicates);
}

proto_msg::CacheResponse
TripleMatchesPartStreamer::get_loaded_predicates_response() {
  proto_msg::CacheResponse cache_response;
  cache_response.set_response_type(
      proto_msg::MessageType::EXTRACT_SEPARATE_PREDICATES_RESPONSE);
  auto *sep_resp =
      cache_response.mutable_extract_separate_predicates_response();
  sep_resp->set_id(channel_id);

  for (auto predicate : loaded_predicates) {
    auto *av_predicate = sep_resp->mutable_available_predicates()->Add();
    av_predicate->set_encoded_data(predicate);
  }

  return cache_response;
}
I_TRMatchingStreamer &
TripleMatchesPartStreamer::start_streaming_matching_triples(
    const proto_msg::TripleNodeIdEnc &triple_pattern) {

  std::unique_ptr<I_TRMatchingStreamer> streamer;

  auto predicate_id = triple_pattern.predicate().encoded_data();
  auto predicate_id_translated =
      (unsigned long)cache->get_nodes_sequence().get_id((long)predicate_id);
  auto fic_response = fully_indexed_cache.get(predicate_id_translated);
  if (fic_response.exists()) {
    streamer = std::make_unique<StreamerFromCachedSource>(
        fic_response.get(), channel_id, current_pattern_channel_id,
        triple_pattern, cache, threshold_part_size);
  } else {
    streamer = std::make_unique<TriplePatternMatchingStreamer>(
        channel_id, current_pattern_channel_id, triple_pattern, cache,
        threshold_part_size);
  }

  auto *ptr = streamer.get();
  triples_streamers_map[current_pattern_channel_id] = std::move(streamer);
  current_pattern_channel_id++;
  return *ptr;
}
void TripleMatchesPartStreamer::clean_pattern_streamer(int pattern_channel_id) {
  std::lock_guard lg(mutex);
  triples_streamers_map[pattern_channel_id] = nullptr;
  triples_streamers_map.erase(pattern_channel_id);
}
bool TripleMatchesPartStreamer::is_done() { return done; }
I_TRMatchingStreamer &
TripleMatchesPartStreamer::get_triple_pattern_streamer(int pattern_channel_id) {
  std::lock_guard lg(mutex);
  return *triples_streamers_map[pattern_channel_id];
}
void TripleMatchesPartStreamer::init_cached_predicate_sources() {
  fully_indexed_cache.init_streamer_predicates(loaded_predicates);
}
} // namespace k2cache
