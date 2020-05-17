//
// Created by Cristobal Miranda, 2020
//

#include "CacheServerTaskProcessor.hpp"

void CacheServerTaskProcessor::process_request(int client_socket_fd,
                                               Message &&message) {
  auto server_task_uptr =
      std::make_unique<ServerTask>(client_socket_fd, std::move(message), cache);

  std::lock_guard<std::mutex> lock_guard(mutex);
  server_tasks.push(std::move(server_task_uptr));
}

std::unique_ptr<ServerTask> CacheServerTaskProcessor::get_server_task() {
  std::lock_guard<std::mutex> lock_guard(mutex);

  if (server_tasks.empty()) {
    return nullptr;
  }

  auto server_task_uptr = std::move(server_tasks.front());
  server_tasks.pop();

  return server_task_uptr;
}

CacheServerTaskProcessor::CacheServerTaskProcessor(Cache &cache)
    : cache(cache) {}
