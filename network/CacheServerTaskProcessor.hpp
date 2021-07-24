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
#include "ServerTask.hpp"
#include "ServerWorker.hpp"
#include "TCPServerConnection.hpp"
#include "TaskProcessor.hpp"
#include <query_processing/QueryResultIterator.hpp>

class CacheServerTaskProcessor : public TaskProcessor {
  using worker_t = ServerWorker<CacheServerTaskProcessor>;

  std::queue<std::unique_ptr<ServerTask>> server_tasks;
  std::mutex mutex;
  Cache &cache;
  uint8_t workers_count;
  std::vector<std::unique_ptr<worker_t>> workers;

  std::unordered_map<int, std::unique_ptr<I_QRStreamer>> streamer_map;

  int current_id;

  static constexpr size_t DEFAULT_THRESHOLD_PART_SZ = 1'000'000;

public:
  explicit CacheServerTaskProcessor(Cache &cache, uint8_t workers_count);

  void process_request(int client_socket_fd);

  std::unique_ptr<ServerTask> get_server_task();
  bool tasks_available();

  void start_workers(TCPServerConnection<CacheServerTaskProcessor> &connection);

  void notify_workers();
  I_QRStreamer &get_streamer(int id) override;
  bool has_streamer(int id) override;
  I_QRStreamer &
  create_streamer(std::shared_ptr<QueryResultIterator> query_result_iterator,
                  std::unique_ptr<TimeControl> &&time_control) override;
  void clean_streamer(int id) override;
};

#endif // RDFCACHEK2_CACHESERVERTASKPROCESSOR_HPP
