//
// Created by Cristobal Miranda, 2020
//

#include "CacheServerTaskProcessor.hpp"

void CacheServerTaskProcessor::process_request(int client_socket_fd) {
  auto server_task_uptr =
      std::make_unique<ServerTask>(client_socket_fd, cache, *this);

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

CacheServerTaskProcessor::CacheServerTaskProcessor(Cache &cache,
                                                   uint8_t workers_count)
    : cache(cache), workers_count(workers_count), current_id(0) {}

void CacheServerTaskProcessor::start_workers(
    TCPServerConnection<CacheServerTaskProcessor> &connection) {
  for (auto i = 0; i < workers_count; i++) {
    workers.emplace_back(
        std::make_unique<worker_t>(connection.get_processor()));
  }
}

void CacheServerTaskProcessor::notify_workers() {
  for (auto &worker_ptr : workers) {
    worker_ptr->notify();
  }
}

bool CacheServerTaskProcessor::tasks_available() {
  std::lock_guard<std::mutex> lock_guard(mutex);
  return !server_tasks.empty();
}
QueryResultStreamer &CacheServerTaskProcessor::get_streamer(int id) {
  return *streamer_map[id];
}

QueryResultStreamer &CacheServerTaskProcessor::create_streamer(
    std::set<uint64_t> &&keys, std::shared_ptr<QueryResult> query_result) {
  int next_id = current_id++;
  std::cout << "creating streamer with id " << next_id << std::endl;
  streamer_map[next_id] = std::make_unique<QueryResultStreamer>(
      std::move(keys), std::move(query_result), next_id, &cache.get_pcm());
  return *streamer_map[next_id];
}
bool CacheServerTaskProcessor::has_streamer(int id) {
  return streamer_map.find(id) != streamer_map.end();
}
void CacheServerTaskProcessor::clean_streamer(int id) {
  streamer_map.erase(id);
}
