//
// Created by cristobal on 8/31/21.
//

#include "TripleMatchesPartStreamer.hpp"

#include "TriplePatternMatchingStreamer.hpp"
#include <map>

TripleMatchesPartStreamer::TripleMatchesPartStreamer(
    int channel_id, std::vector<unsigned long> &&loaded_predicates,
    size_t threshold_part_size, TaskProcessor *task_processor, Cache *cache)
    : channel_id(channel_id), loaded_predicates(std::move(loaded_predicates)),
      threshold_part_size(threshold_part_size), cache(cache),
      task_processor(task_processor), current_pattern_channel_id(0),
      done(false) {}

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
    // auto resource = cache->extract_resource(predicate);
    auto *av_predicate = sep_resp->mutable_available_predicates()->Add();
    av_predicate->set_encoded_data(predicate);
  }

  return cache_response;
}
I_TRMatchingStreamer &
TripleMatchesPartStreamer::start_streaming_matching_triples(
    const proto_msg::TripleNodeIdEnc &triple_pattern) {


  auto streamer = get_streamer(triple_pattern);
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
std::unique_ptr<I_TRMatchingStreamer> TripleMatchesPartStreamer::get_streamer(
    const proto_msg::TripleNodeIdEnc &triple_pattern) {
  if(should_load_completely(triple_pattern)){
    return get_full_streamer(triple_pattern);
  }
  return std::make_unique<TriplePatternMatchingStreamer>(
        channel_id, current_pattern_channel_id, triple_pattern, cache,
        threshold_part_size);
}
bool TripleMatchesPartStreamer::should_load_completely(
    const proto_msg::TripleNodeIdEnc &triple_pattern) {

  auto predicate_id = triple_pattern.predicate().encoded_data();
  auto predicate_id_translated =
      (unsigned long)cache->get_nodes_sequence().get_id((long)predicate_id);

  auto fetch_result =
      cache->get_pcm().get_predicates_index_cache().fetch_k2tree(
          predicate_id_translated);
  if (!fetch_result.exists()) {
    return true;
  }

  auto &k2tree = fetch_result.get();
  static constexpr unsigned long max_points = 6'250'000;
  return k2tree.size() <= max_points;
}
std::unique_ptr<I_TRMatchingStreamer>
TripleMatchesPartStreamer::get_full_streamer(
    const proto_msg::TripleNodeIdEnc &triple_pattern) {


  return std::make_unique<FullyLoadedStreamer>(
      channel_id, current_pattern_channel_id, triple_pattern, cache,
      threshold_part_size);
}
std::unique_ptr<I_TRMatchingStreamer>
TripleMatchesPartStreamer::create_null_streamer() {
  return std::unique_ptr<I_TRMatchingStreamer>();
}
