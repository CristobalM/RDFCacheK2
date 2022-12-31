//
// Created by cristobal on 04-08-21.
//

#include "ReplacementTask.hpp"
namespace k2cache {
void ReplacementTask::process() {
  auto &replacement = cache.get_replacement();
  for (auto p : *predicates) {
    auto predicate_sz = cache.get_pcm().get_predicate_size(p);
    replacement.hit_key(p, predicate_sz);
  }
}

ReplacementTask::ReplacementTask(
    CacheContainer &cache,
    std::shared_ptr<const std::vector<uint64_t>> predicates)
    : cache(cache), predicates(std::move(predicates)) {}
} // namespace k2cache