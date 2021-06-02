//
// Created by Cristobal Miranda, 2020
//

#ifndef RDFCACHEK2_CACHESERVERTASKPROCESSOR_HPP
#define RDFCACHEK2_CACHESERVERTASKPROCESSOR_HPP

#include <memory>
#include <mutex>
#include <queue>

#include <Cache.hpp>

#include "Message.hpp"
#include "QueryResultStreamer.hpp"
#include "ServerTask.hpp"
#include "ServerWorker.hpp"
#include "TCPServerConnection.hpp"
#include "TaskProcessor.hpp"

class CacheServerTaskProcessor : public TaskProcessor {
  using worker_t = ServerWorker<CacheServerTaskProcessor>;

  std::queue<std::unique_ptr<ServerTask>> server_tasks;
  std::mutex mutex;
  Cache &cache;
  uint8_t workers_count;
  std::vector<std::unique_ptr<worker_t>> workers;

  std::unordered_map<int, QueryResultStreamer> streamer_map;

  int current_id;

public:
  explicit CacheServerTaskProcessor(Cache &cache, uint8_t workers_count);

  void process_request(int client_socket_fd);

  std::unique_ptr<ServerTask> get_server_task();
  bool tasks_available();

  void start_workers(TCPServerConnection<CacheServerTaskProcessor> &connection);

  void notify_workers();
  QueryResultStreamer &get_streamer(int id) override;
  QueryResultStreamer &create_streamer(std::set<uint64_t> &&keys,
                                       QueryResult &&query_result) override;
};

#endif // RDFCACHEK2_CACHESERVERTASKPROCESSOR_HPP
