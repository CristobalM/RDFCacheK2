//
// Created by Cristobal Miranda, 2020
//

#include "CacheContainerImpl.hpp"
#include "manager/PCMFactory.hpp"
#include "nodeids/NodeIdsManagerFactory.hpp"
#include "replacement/CacheReplacement.hpp"
#include "replacement/CacheReplacementFactory.hpp"

namespace k2cache {
CacheContainerImpl::CacheContainerImpl(const CacheArgs &args)
    : pcm( PCMFactory::create(args)),
      cache_replacement(CacheReplacementFactory::create_cache_replacement(
          args.memory_budget_bytes, this->pcm.get(),
          args.replacement_strategy)),
      strategy_id(args.replacement_strategy) {
  if (args.replacement_strategy ==
      I_CacheReplacement::REPLACEMENT_STRATEGY::NO_CACHING) {
    std::cout << "Loading all predicates..." << std::endl;
    auto start = std::chrono::high_resolution_clock::now();
    this->pcm->load_all_predicates();
    auto end = std::chrono::high_resolution_clock::now();
    auto duration =
        std::chrono::duration_cast<std::chrono::seconds>(end - start);
    std::cout << "Done loading all predicates, took " << duration.count()
              << " seconds" << std::endl;
  }
}

PredicatesCacheManager &CacheContainerImpl::get_pcm() {
  return *pcm;
}

I_CacheReplacement &CacheContainerImpl::get_replacement() {
  return *cache_replacement;
}

I_CacheReplacement::REPLACEMENT_STRATEGY CacheContainerImpl::get_strategy_id() {
  return strategy_id;
}
std::vector<unsigned long>
CacheContainerImpl::extract_loaded_predicates_from_sequence(
    const std::vector<unsigned long> &input_predicates_ids) {
  std::vector<unsigned long> result;
  auto &predicates_index_cache = pcm->get_predicates_index_cache();

  for (auto current_predicate_id : input_predicates_ids) {
    if (predicates_index_cache.has_predicate_active(current_predicate_id))
      result.push_back(current_predicate_id);
  }

  return result;
}

NodeIdsManager &CacheContainerImpl::get_nodes_ids_manager() {
  return pcm->get_nodes_ids_manager();
}

} // namespace k2cache