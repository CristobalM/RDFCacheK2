//
// Created by cristobal on 8/31/21.
//

#ifndef RDFCACHEK2_TRIPLEMATCHESPARTSTREAMER_HPP
#define RDFCACHEK2_TRIPLEMATCHESPARTSTREAMER_HPP

#include "CacheContainer.hpp"
#include "I_TRStreamer.hpp"
#include "fic/FullyIndexedCache.hpp"
#include "k2tree/K2TreeScanner.hpp"
#include "server/tasks/TaskProcessor.hpp"
#include <memory>
#include <mutex>
namespace k2cache {
class TripleMatchesPartStreamer : public I_TRStreamer {
  int channel_id;
  std::vector<unsigned long> loaded_predicates;
  size_t threshold_part_size;
  CacheContainer *cache;
  TaskProcessor *task_processor;

  int current_pattern_channel_id;

  std::map<int, std::unique_ptr<I_TRMatchingStreamer>> triples_streamers_map;

  bool done;

  std::mutex mutex;

  FullyIndexedCache &fully_indexed_cache;

public:
  TripleMatchesPartStreamer(int channel_id,
                            std::vector<unsigned long> &&loaded_predicates,
                            size_t threshold_part_size,
                            TaskProcessor *task_processor,
                            CacheContainer *cache,
                            FullyIndexedCache &fully_indexed_cache);

  const std::vector<unsigned long> &get_predicates_in_use() override;
  int get_id() override;
  proto_msg::CacheResponse get_loaded_predicates_response() override;

  ~TripleMatchesPartStreamer() override;
  I_TRMatchingStreamer &
  start_streaming_matching_triples(const TripleNodeId &triple_pattern) override;
  void clean_pattern_streamer(int pattern_channel_id) override;
  bool is_done() override;
  I_TRMatchingStreamer &
  get_triple_pattern_streamer(int pattern_channel_id) override;

private:
  proto_msg::CacheResponse time_control_finished_error();
  proto_msg::CacheResponse timeout_proto();
  void set_finished();
  void init_cached_predicate_sources();
};
} // namespace k2cache
#endif // RDFCACHEK2_TRIPLEMATCHESPARTSTREAMER_HPP
