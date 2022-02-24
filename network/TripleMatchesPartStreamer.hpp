//
// Created by cristobal on 8/31/21.
//

#ifndef RDFCACHEK2_TRIPLEMATCHESPARTSTREAMER_HPP
#define RDFCACHEK2_TRIPLEMATCHESPARTSTREAMER_HPP

#include "I_TRStreamer.hpp"
#include "TaskProcessor.hpp"
#include <Cache.hpp>
#include <K2TreeScanner.hpp>
#include <mutex>
class TripleMatchesPartStreamer : public I_TRStreamer {
  int channel_id;
  std::vector<unsigned long> loaded_predicates;
  size_t threshold_part_size;
  Cache *cache;
  TaskProcessor *task_processor;

  int current_pattern_channel_id;

  std::map<int, std::unique_ptr<I_TRMatchingStreamer>> triples_streamers_map;

  bool done;

  std::mutex mutex;

  std::unordered_map<unsigned long, >

public:
  TripleMatchesPartStreamer(int channel_id,
                            std::vector<unsigned long> &&loaded_predicates,
                            size_t threshold_part_size,
                            TaskProcessor *task_processor, Cache *cache);

  const std::vector<unsigned long> &get_predicates_in_use() override;
  int get_id() override;
  proto_msg::CacheResponse get_loaded_predicates_response() override;

  ~TripleMatchesPartStreamer() override;
  I_TRMatchingStreamer &start_streaming_matching_triples(
      const proto_msg::TripleNodeIdEnc &triple_pattern) override;
  void clean_pattern_streamer(int pattern_channel_id) override;
  bool is_done() override;
  I_TRMatchingStreamer &
  get_triple_pattern_streamer(int pattern_channel_id) override;

private:
  proto_msg::CacheResponse time_control_finished_error();
  proto_msg::CacheResponse timeout_proto();
  void set_finished();
  std::unique_ptr<I_TRMatchingStreamer>
  get_streamer(const proto_msg::TripleNodeIdEnc &triple_pattern);
  bool should_load_completely(const proto_msg::TripleNodeIdEnc &triple_pattern);
  std::unique_ptr<I_TRMatchingStreamer>
  get_full_streamer(const proto_msg::TripleNodeIdEnc &triple_pattern);
  std::unique_ptr<I_TRMatchingStreamer> create_null_streamer();
};

#endif // RDFCACHEK2_TRIPLEMATCHESPARTSTREAMER_HPP
