//
// Created by cristobal on 10-07-22.
//

#ifndef RDFCACHEK2_TESTINGTASKPROCESSOR_HPP
#define RDFCACHEK2_TESTINGTASKPROCESSOR_HPP

#include "server/replacement/ReplacementTaskProcessor.hpp"
#include <CacheContainer.hpp>
#include <memory>
#include <server/tasks/TaskProcessor.hpp>
#include <streaming/I_TRStreamer.hpp>
#include <unordered_map>
#include <vector>

namespace k2cache {
class TestingTaskProcessor : public TaskProcessor {
  CacheContainer &cache;
  int current_triples_streamers_channel_id = 0;
  std::unordered_map<int, std::unique_ptr<I_TRStreamer>> triples_streamer_map;

  static constexpr size_t DEFAULT_THRESHOLD_PART_SZ = 100'000'000;

  std::unordered_map<int, std::unique_ptr<Updater>> updaters_sessions;
  int current_update_session_id;


public:
  explicit TestingTaskProcessor(CacheContainer &cache);

  I_TRStreamer &get_triple_streamer(int id) override;
  bool has_triple_streamer(int channel_id) override;
  void clean_triple_streamer(int id) override;
  void process_missed_predicates(
      std::shared_ptr<const std::vector<unsigned long>> predicates) override;
  void mark_using(const std::vector<unsigned long> &predicates) override;
  void mark_ready(const std::vector<unsigned long> &predicates_in_use) override;
  I_TRStreamer &create_triples_streamer(
      std::vector<unsigned long> &&loaded_predicates) override;
  int begin_update_session() override;
  Updater &get_updater(int updater_id) override;
  void log_updates(std::vector<K2TreeUpdates> &k2trees_updates) override;
  WriteDataLock acquire_write_lock() override;
  void sync_to_persistent() override;
};
} // namespace k2cache

#endif // RDFCACHEK2_TESTINGTASKPROCESSOR_HPP
