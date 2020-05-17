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
#include "ServerTask.hpp"

class CacheServerTaskProcessor {
  std::queue<std::unique_ptr<ServerTask>> server_tasks;

  std::mutex mutex;

  Cache &cache;

public:
  CacheServerTaskProcessor(Cache &cache);

  void process_request(int client_socket_fd, Message &&message);

  std::unique_ptr<ServerTask> get_server_task();
};

#endif // RDFCACHEK2_CACHESERVERTASKPROCESSOR_HPP
