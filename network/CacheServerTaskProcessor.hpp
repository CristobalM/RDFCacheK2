//
// Created by Cristobal Miranda, 2020
//

#ifndef RDFCACHEK2_CACHESERVERTASKPROCESSOR_HPP
#define RDFCACHEK2_CACHESERVERTASKPROCESSOR_HPP

#include <Cache.hpp>
#include <memory>
#include <mutex>
#include <queue>

#include "Message.hpp"
#include "ReplacementTaskProcessor.hpp"
#include "ServerTask.hpp"
#include "ServerWorker.hpp"
#include "TCPServerConnection.hpp"
#include "TaskProcessor.hpp"
#include <query_processing/QueryResultIteratorHolder.hpp>

class CacheServerTaskProcessor : public TaskProcessor {
  using worker_t = ServerWorker<CacheServerTaskProcessor>;

  std::queue<std::unique_ptr<ServerTask>> server_tasks;
  std::mutex mutex;
  Cache &cache;
  uint8_t workers_count;
  std::vector<std::unique_ptr<worker_t>> workers;

  std::unordered_map<int, std::unique_ptr<I_QRStreamer>> streamer_map;

  int current_id;

  //static constexpr size_t DEFAULT_THRESHOLD_PART_SZ = 100'000'000; // 100 MB
  static constexpr size_t DEFAULT_THRESHOLD_PART_SZ = 10'000'000; // 10 MB
  // static constexpr size_t DEFAULT_THRESHOLD_PART_SZ = 1'024;

  ReplacementTaskProcessor replacement_task_processor;
  // std::unique_ptr<ServerWorker<ReplacementTaskProcessor>> replacement_worker;

public:
  explicit CacheServerTaskProcessor(Cache &cache, uint8_t workers_count);

  void process_request(int client_socket_fd);

  std::unique_ptr<ServerTask> get_server_task();
  bool tasks_available();

  void start_workers(TCPServerConnection<CacheServerTaskProcessor> &connection);

  void notify_workers();
  I_QRStreamer &get_streamer(int id) override;
  bool has_streamer(int id) override;
  I_QRStreamer &create_streamer(
      std::shared_ptr<QueryResultIteratorHolder> query_result_iterator,
      std::unique_ptr<TimeControl> &&time_control,
      std::shared_ptr<const std::vector<unsigned long>> predicates_in_use)
      override;
  void clean_streamer(int id) override;
  void process_missed_predicates(
      std::shared_ptr<const std::vector<unsigned long>> predicates) override;
  void mark_using(const std::vector<unsigned long> &predicates) override;
  void mark_ready(const std::vector<unsigned long> &predicates_in_use) override;
};

#endif // RDFCACHEK2_CACHESERVERTASKPROCESSOR_HPP
