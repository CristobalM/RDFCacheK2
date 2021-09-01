//
// Created by cristobal on 8/31/21.
//

#ifndef RDFCACHEK2_TRIPLEMATCHESPARTSTREAMER_HPP
#define RDFCACHEK2_TRIPLEMATCHESPARTSTREAMER_HPP

#include <mutex>
#include "I_TRStreamer.hpp"
#include "TaskProcessor.hpp"
#include <Cache.hpp>
#include <K2TreeScanner.hpp>
#include <TimeControl.hpp>
class TripleMatchesPartStreamer : public I_TRStreamer {
  int channel_id;
  std::vector<unsigned long> loaded_predicates;
  size_t threshold_part_size;
  std::unique_ptr<TimeControl> time_control;
  Cache *cache;
  TaskProcessor *task_processor;

  int current_pattern_channel_id;

  std::map<int, std::unique_ptr<I_TRMatchingStreamer>> triples_streamers_map;

  bool done;

  std::mutex mutex;

public:
  TripleMatchesPartStreamer(int channel_id,
                            std::vector<unsigned long> &&loaded_predicates,
                            size_t threshold_part_size,
                            std::unique_ptr<TimeControl> &&time_control,
                            TaskProcessor *task_processor, Cache *cache);

  const std::vector<unsigned long> &get_predicates_in_use() override;
  int get_id() override;
  proto_msg::CacheResponse get_loaded_predicates_response() override;

  ~TripleMatchesPartStreamer() override;
  I_TRMatchingStreamer &start_streaming_matching_triples(
      const proto_msg::TripleNode &triple_pattern) override;
  void clean_pattern_streamer(int pattern_channel_id) override;
  bool is_done() override;
  I_TRMatchingStreamer &
  get_triple_pattern_streamer(int pattern_channel_id) override;

private:
  proto_msg::CacheResponse time_control_finished_error();
  proto_msg::CacheResponse timeout_proto();
  void set_finished();
};

#endif // RDFCACHEK2_TRIPLEMATCHESPARTSTREAMER_HPP
