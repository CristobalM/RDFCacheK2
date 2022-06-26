//
// Created by Cristobal Miranda, 2020
//

#include <replacement/LRUReplacementStrategy.hpp>
#include <utility>

#include "Cache.hpp"
#include "FileRWHandler.hpp"
#include "replacement/CacheReplacement.hpp"
#include "replacement/CacheReplacementFactory.hpp"
namespace k2cache {
Cache::Cache(std::shared_ptr<PredicatesCacheManager> predicates_cache_manager,
             const CacheArgs &args)
    : cache_manager(std::move(predicates_cache_manager)),
      nodes_sequence(std::make_unique<NodesSequence>(
          NodesSequence::from_file(args.node_ids_filename))),
      cache_replacement(CacheReplacementFactory::create_cache_replacement(
          args.memory_budget_bytes, cache_manager.get(),
          args.replacement_strategy)),
      strategy_id(args.replacement_strategy) {}

PredicatesCacheManager &Cache::get_pcm() { return *cache_manager; }

I_CacheReplacement &Cache::get_replacement() { return *cache_replacement; }

I_CacheReplacement::REPLACEMENT_STRATEGY Cache::get_strategy_id() {
  return strategy_id;
}
std::vector<unsigned long> Cache::extract_loaded_predicates_from_sequence(
    const std::vector<unsigned long> &input_predicates_ids) {
  std::vector<unsigned long> result;
  auto &predicates_index_cache = cache_manager->get_predicates_index_cache();

  for (auto current_predicate_id : input_predicates_ids) {
    if (predicates_index_cache.has_predicate_active(current_predicate_id))
      result.push_back(current_predicate_id);
  }

  return result;
}

NodesSequence &Cache::get_nodes_sequence() { return *nodes_sequence; }
} // namespace k2cache