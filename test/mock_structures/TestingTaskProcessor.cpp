//
// Created by cristobal on 10-07-22.
//

#include "TestingTaskProcessor.hpp"
#include "streaming/TripleMatchesPartStreamer.hpp"
#include <iostream>
#include <memory>
#include <server/tasks/WriteDataLock.hpp>
#include <vector>

namespace k2cache {
I_TRStreamer &TestingTaskProcessor::get_triple_streamer(int) {
  throw std::runtime_error("not implemented get_triple_streamer");
}
bool TestingTaskProcessor::has_triple_streamer(int) {
  throw std::runtime_error("not implemented has_triple_streamer");
}
void TestingTaskProcessor::clean_triple_streamer(int) {
  throw std::runtime_error("not implemented clean_triple_streamer");
}
void TestingTaskProcessor::process_missed_predicates(
    std::shared_ptr<const std::vector<unsigned long>>) {
  throw std::runtime_error("not implemented process_missed_predicates");
}
void TestingTaskProcessor::mark_using(const std::vector<unsigned long> &) {
  throw std::runtime_error("not implemented mark_using");
}
void TestingTaskProcessor::mark_ready(const std::vector<unsigned long> &) {}
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
  throw std::runtime_error("not implemented begin_update_session");
}
Updater &TestingTaskProcessor::get_updater(int) {
  throw std::runtime_error("not implemented get_updater");
}
void TestingTaskProcessor::log_updates(std::vector<K2TreeUpdates> &updates) {
  cache.get_pcm().get_updates_logger().log(updates);
}
WriteDataLock TestingTaskProcessor::acquire_write_lock() { return {}; }
void TestingTaskProcessor::sync_to_persistent() {
  throw std::runtime_error("not implemented sync_to_persistent");
}
TestingTaskProcessor::TestingTaskProcessor(CacheContainer &cache)
    : cache(cache) {}

} // namespace k2cache