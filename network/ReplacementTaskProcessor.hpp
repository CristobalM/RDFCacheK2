//
// Created by cristobal on 04-08-21.
//

#ifndef RDFCACHEK2_REPLACEMENTTASKPROCESSOR_HPP
#define RDFCACHEK2_REPLACEMENTTASKPROCESSOR_HPP

#include "ReplacementTask.hpp"
#include "ServerWorker.hpp"
#include <memory>
#include <mutex>
#include <queue>
class ReplacementTaskProcessor {
  using worker_t = ServerWorker<ReplacementTaskProcessor>;
  std::mutex m;
  std::queue<std::unique_ptr<ReplacementTask>> tasks;
  Cache &cache;
  std::unique_ptr<worker_t> worker;

public:
  ReplacementTaskProcessor(Cache &cache);
  bool tasks_available();
  std::unique_ptr<ReplacementTask> get_server_task();
  void add_task(std::vector<unsigned long> &&predicates);
  void notify();
  void mark_used(std::vector<unsigned long> &predicates);
  void mark_ready(std::vector<unsigned long> &predicates_in_use);
};

#endif // RDFCACHEK2_REPLACEMENTTASKPROCESSOR_HPP
