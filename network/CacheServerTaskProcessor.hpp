//
// Created by Cristobal Miranda, 2020
//

#ifndef RDFCACHEK2_CACHESERVERTASKPROCESSOR_HPP
#define RDFCACHEK2_CACHESERVERTASKPROCESSOR_HPP

#include <Cache.hpp>
#include <memory>
#include <mutex>
#include <queue>

#include "I_TRStreamer.hpp"
#include "I_Updater.hpp"
#include "Message.hpp"
#include "ReplacementTaskProcessor.hpp"
#include "ServerTask.hpp"
#include "ServerWorker.hpp"
#include "TCPServerConnection.hpp"
#include "TaskProcessor.hpp"
#include "UpdatesLogger.hpp"

class CacheServerTaskProcessor : public TaskProcessor {
  using worker_t = ServerWorker<CacheServerTaskProcessor>;

  std::queue<std::unique_ptr<ServerTask>> server_tasks;
  std::mutex mutex;
  Cache &cache;
  uint8_t workers_count;
  std::vector<std::unique_ptr<worker_t>> workers;

  static constexpr size_t DEFAULT_THRESHOLD_PART_SZ = 100'000'000; // 100 MB
  // static constexpr size_t DEFAULT_THRESHOLD_PART_SZ = 1'000'000; // 1 MB
  // static constexpr size_t DEFAULT_THRESHOLD_PART_SZ = 1'000'000'000; // 1 GB
  // static constexpr size_t DEFAULT_THRESHOLD_PART_SZ = 1'024;

  ReplacementTaskProcessor replacement_task_processor;
  // std::unique_ptr<ServerWorker<ReplacementTaskProcessor>> replacement_worker;

  int current_triples_streamers_channel_id;
  std::unordered_map<int, std::unique_ptr<I_TRStreamer>> triples_streamer_map;

  std::unordered_map<int, std::unique_ptr<I_Updater>> updaters_sessions;
  int current_update_session_id;

public:
  explicit CacheServerTaskProcessor(Cache &cache, uint8_t workers_count);

  void process_request(int client_socket_fd);

  std::unique_ptr<ServerTask> get_server_task();
  bool tasks_available();

  void start_workers(TCPServerConnection<CacheServerTaskProcessor> &connection);

  void notify_workers();

  void process_missed_predicates(
      std::shared_ptr<const std::vector<unsigned long>> predicates) override;
  void mark_using(const std::vector<unsigned long> &predicates) override;
  void mark_ready(const std::vector<unsigned long> &predicates_in_use) override;
  I_TRStreamer &create_triples_streamer(
      std::vector<unsigned long> &&loaded_predicates) override;
  I_TRStreamer &get_triple_streamer(int channel_id) override;
  bool has_triple_streamer(int channel_id) override;
  void clean_triple_streamer(int channel_id) override;
  int begin_update_session() override;
  I_Updater &get_updater(int updater_id) override;
  void log_updates(std::vector<K2TreeUpdates> &k2trees_updates) override;
  WriteDataLock acquire_write_lock() override;
  ~CacheServerTaskProcessor() override;
  void sync_to_persistent() override;
  void sync_logs_to_indexes();
};

#endif // RDFCACHEK2_CACHESERVERTASKPROCESSOR_HPP
