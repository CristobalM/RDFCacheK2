//
// Created by cristobal on 04-08-21.
//

#ifndef RDFCACHEK2_REPLACEMENTTASKPROCESSOR_HPP
#define RDFCACHEK2_REPLACEMENTTASKPROCESSOR_HPP

#include "ReplacementTask.hpp"
#include "server/ServerWorker.hpp"
#include <memory>
#include <mutex>
#include <queue>
namespace k2cache {
class ReplacementTaskProcessor {
  using worker_t = ServerWorker<ReplacementTaskProcessor>;
  std::mutex m;
  std::queue<std::unique_ptr<ReplacementTask>> tasks;
  CacheContainer &cache;
  std::unique_ptr<worker_t> worker;

public:
  explicit ReplacementTaskProcessor(CacheContainer &cache);
  bool tasks_available();
  std::unique_ptr<ReplacementTask> get_server_task();
  void add_task(std::shared_ptr<const std::vector<unsigned long>> predicates);
  void notify();
  void mark_used(const std::vector<unsigned long> &predicates);
  void mark_ready(const std::vector<unsigned long> &predicates_in_use);
};
} // namespace k2cache
#endif // RDFCACHEK2_REPLACEMENTTASKPROCESSOR_HPP
