//
// Created by cristobal on 8/31/21.
//

#include "TripleMatchesPartStreamer.hpp"

#include "CacheServerTaskProcessor.hpp"
#include "TriplePatternMatchingStreamer.hpp"
#include <map>

TripleMatchesPartStreamer::TripleMatchesPartStreamer(
    int channel_id, std::vector<unsigned long> &&loaded_predicates,
    size_t threshold_part_size, std::unique_ptr<TimeControl> &&time_control,
    TaskProcessor *task_processor, Cache *cache)
    : channel_id(channel_id), loaded_predicates(std::move(loaded_predicates)),
      threshold_part_size(threshold_part_size),
      time_control(std::move(time_control)), cache(cache),
      task_processor(task_processor), first(true), second(true), done(false),
      current_pattern_channel_id(0) {}

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
  if (!time_control->has_error())
    return timeout_proto();
  set_finished();
  std::cerr << "Query stopped early due to error: "
            << time_control->get_query_error().get_str() << std::endl;
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

  auto *sep_resp =
      cache_response.mutable_extract_separate_predicates_response();
  sep_resp->set_id(channel_id);

  for (auto predicate : loaded_predicates) {
    auto resource = cache->extract_resource(predicate);
    proto_msg::RDFTerm term;
    term.set_term_type(resource.get_proto_type());
    term.set_term_value(std::move(resource.value));
    sep_resp->mutable_available_predicates()->Add(std::move(term));
  }

  return cache_response;
}
I_TRMatchingStreamer &
TripleMatchesPartStreamer::start_streaming_matching_triples(
    const proto_msg::TripleNode &triple_pattern) {

  auto streamer = std::make_unique<TriplePatternMatchingStreamer>(
      channel_id, current_pattern_channel_id,
      triple_pattern,
      cache,
      task_processor,
      time_control.get(),
      threshold_part_size);
  auto *ptr = streamer.get();
  triples_streamers_map[current_pattern_channel_id] = std::move(streamer);
  current_pattern_channel_id++;
  return *ptr;
}
