//
// Created by Cristobal Miranda, 2020
//

#include "CacheServerTaskProcessor.hpp"
#include "QueryResultPartStreamer.hpp"
#include "TripleMatchesPartStreamer.hpp"
#include "UpdaterSession.hpp"

#include <iostream>

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
    : cache(cache), workers_count(workers_count), current_id(0),
      replacement_task_processor(cache),
      current_triples_streamers_channel_id(0), current_update_session_id(0) {}

void CacheServerTaskProcessor::start_workers(
    TCPServerConnection<CacheServerTaskProcessor> &connection) {
  std::lock_guard lg(mutex);
  for (auto i = 0; i < workers_count; i++) {
    workers.emplace_back(
        std::make_unique<worker_t>(connection.get_processor()));
  }
}

void CacheServerTaskProcessor::notify_workers() {
  std::lock_guard lg(mutex);
  for (auto &worker_ptr : workers) {
    worker_ptr->notify();
  }
}

bool CacheServerTaskProcessor::tasks_available() {
  std::lock_guard<std::mutex> lock_guard(mutex);
  return !server_tasks.empty();
}
I_QRStreamer &CacheServerTaskProcessor::get_streamer(int id) {
  std::lock_guard lg(mutex);
  return *streamer_map[id];
}

bool CacheServerTaskProcessor::has_streamer(int id) {
  std::lock_guard lg(mutex);

  return streamer_map.find(id) != streamer_map.end();
}

I_QRStreamer &CacheServerTaskProcessor::create_streamer(
    std::shared_ptr<QueryResultIteratorHolder> query_result_iterator,
    std::unique_ptr<TimeControl> &&time_control,
    std::shared_ptr<const std::vector<unsigned long>> predicates_in_use) {
  std::lock_guard lg(mutex);

  streamer_map[current_id] = std::make_unique<QueryResultPartStreamer>(
      current_id, std::move(query_result_iterator), std::move(time_control),
      DEFAULT_THRESHOLD_PART_SZ, std::move(predicates_in_use), this);

  auto last_created = current_id;
  current_id++;
  return *streamer_map[last_created];
}

void CacheServerTaskProcessor::clean_streamer(int id) {
  std::lock_guard lg(mutex);
  streamer_map[id] = nullptr;
  streamer_map.erase(id);
}
void CacheServerTaskProcessor::process_missed_predicates(
    std::shared_ptr<const std::vector<unsigned long>> predicates) {
  replacement_task_processor.add_task(std::move(predicates));
}

void CacheServerTaskProcessor::mark_using(
    const std::vector<unsigned long> &predicates) {
  replacement_task_processor.mark_used(predicates);
}
void CacheServerTaskProcessor::mark_ready(
    const std::vector<unsigned long> &predicates_in_use) {
  std::lock_guard lg(cache.get_replacement().get_replacement_mutex());
  replacement_task_processor.mark_ready(predicates_in_use);
}

I_TRStreamer &CacheServerTaskProcessor::create_triples_streamer(
    std::vector<unsigned long> &&loaded_predicates,
    std::unique_ptr<TimeControl> &&time_control) {
  std::lock_guard lg(mutex);

  auto streamer = std::make_unique<TripleMatchesPartStreamer>(
      current_triples_streamers_channel_id, std::move(loaded_predicates),
      DEFAULT_THRESHOLD_PART_SZ, std::move(time_control), this, &cache);
  auto *ptr = streamer.get();
  triples_streamer_map[current_triples_streamers_channel_id] =
      std::move(streamer);

  current_triples_streamers_channel_id++;
  return *ptr;
}
I_TRStreamer &CacheServerTaskProcessor::get_triple_streamer(int channel_id) {
  std::lock_guard lg(mutex);
  return *triples_streamer_map[channel_id];
}
bool CacheServerTaskProcessor::has_triple_streamer(int channel_id) {
  std::lock_guard lg(mutex);
  return triples_streamer_map.find(channel_id) != triples_streamer_map.end();
}
void CacheServerTaskProcessor::clean_triple_streamer(int channel_id) {
  std::lock_guard lg(mutex);
  triples_streamer_map[channel_id] = nullptr;
  triples_streamer_map.erase(channel_id);
}
int CacheServerTaskProcessor::begin_update_session() {
  std::lock_guard lg(mutex);
  int update_id = current_update_session_id;
  updaters_sessions[update_id] =
      std::make_unique<UpdaterSession>(update_id, this, &cache);
  current_update_session_id++;
  return update_id;
}
I_Updater &CacheServerTaskProcessor::get_updater(int updater_id) {
  std::lock_guard lg(mutex);
  return *updaters_sessions[updater_id];
}
