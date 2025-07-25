//
// Created by cristobal on 04-08-21.
//

#include "ReplacementTaskProcessor.hpp"
namespace k2cache {
bool ReplacementTaskProcessor::tasks_available() {
  std::lock_guard lg(m);
  return !tasks.empty();
}
std::unique_ptr<ReplacementTask> ReplacementTaskProcessor::get_server_task() {
  std::lock_guard lg(m);

  if (tasks.empty())
    return nullptr;

  auto next_task = std::move(tasks.front());
  tasks.pop();
  return next_task;
}
void ReplacementTaskProcessor::add_task(
    std::shared_ptr<const std::vector<uint64_t>> predicates) {
  {
    std::lock_guard lg(m);
    tasks.push(std::make_unique<ReplacementTask>(cache, std::move(predicates)));
  }
  notify();
}

ReplacementTaskProcessor::ReplacementTaskProcessor(CacheContainer &cache)
    : cache(cache) {
  worker = std::make_unique<worker_t>(*this);
}

void ReplacementTaskProcessor::notify() { worker->notify(); }
void ReplacementTaskProcessor::mark_used(
    const std::vector<uint64_t> &predicates) {
  auto &replacement = cache.get_replacement();
  for (auto p : predicates) {
    replacement.mark_using(p);
  }
}
void ReplacementTaskProcessor::mark_ready(
    const std::vector<uint64_t> &predicates_in_use) {
  auto &replacement = cache.get_replacement();
  for (auto p : predicates_in_use) {
    replacement.mark_ready(p);
  }
}
} // namespace k2cache
