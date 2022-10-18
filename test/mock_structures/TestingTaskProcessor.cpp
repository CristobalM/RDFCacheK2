//
// Created by cristobal on 10-07-22.
//

#include "TestingTaskProcessor.hpp"
#include "server/session/UpdaterSession.hpp"
#include "streaming/TripleMatchesPartStreamer.hpp"
#include <iostream>
#include <memory>
#include <server/tasks/WriteDataLock.hpp>
#include <vector>

namespace k2cache {
I_TRStreamer &TestingTaskProcessor::get_triple_streamer(int channel_id) {
  return *triples_streamer_map[channel_id];
}
bool TestingTaskProcessor::has_triple_streamer(int channel_id) {
  return triples_streamer_map.find(channel_id) != triples_streamer_map.end();
}
void TestingTaskProcessor::clean_triple_streamer(int channel_id) {
    triples_streamer_map[channel_id] = nullptr;
    triples_streamer_map.erase(channel_id);
}
void TestingTaskProcessor::process_missed_predicates(
    std::shared_ptr<const std::vector<unsigned long>> predicates) {
  ReplacementTask task(cache, std::move(predicates));
  task.process();
}
void TestingTaskProcessor::mark_using(const std::vector<unsigned long> &predicates) {
  for(auto p : predicates){
    cache.get_replacement().mark_using(p);
  }
}
void TestingTaskProcessor::mark_ready(const std::vector<unsigned long> &predicates) {
  for(auto p : predicates){
    cache.get_replacement().mark_ready(p);
  }
}
I_TRStreamer &TestingTaskProcessor::create_triples_streamer(
    std::vector<unsigned long> &&loaded_predicates) {
  auto streamer =
      std::unique_ptr<TripleMatchesPartStreamer>(new TripleMatchesPartStreamer(
          current_triples_streamers_channel_id, std::move(loaded_predicates),
          DEFAULT_THRESHOLD_PART_SZ, this, &cache,
          cache.get_pcm().get_fully_indexed_cache()));
  auto *ptr = streamer.get();
  triples_streamer_map[current_triples_streamers_channel_id] =
      std::move(streamer);

  current_triples_streamers_channel_id++;
  return *ptr;
}
int TestingTaskProcessor::begin_update_session() {
    int update_id = current_update_session_id;
    updaters_sessions[update_id] = std::make_unique<UpdaterSession>(this, &cache);
    current_update_session_id++;
    return update_id;
}
Updater &TestingTaskProcessor::get_updater(int updater_id) {
  return *updaters_sessions[updater_id];
}
void TestingTaskProcessor::log_updates(std::vector<K2TreeUpdates> &updates) {
  cache.get_pcm().get_updates_logger().log(updates);
}
WriteDataLock TestingTaskProcessor::acquire_write_lock() { return {}; }
void TestingTaskProcessor::sync_to_persistent() {
}
TestingTaskProcessor::TestingTaskProcessor(CacheContainer &cache)
    : cache(cache) {}

} // namespace k2cache