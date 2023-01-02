//
// Created by Cristobal Miranda, 2020
//

#include "CacheContainerImpl.hpp"
#include "manager/PCMFactory.hpp"
#include "nodeids/NodeIdsManagerFactory.hpp"
#include "replacement/CacheReplacement.hpp"
#include "replacement/CacheReplacementFactory.hpp"

namespace k2cache {

PredicatesCacheManager &CacheContainerImpl::get_pcm() { return *pcm; }

I_CacheReplacement &CacheContainerImpl::get_replacement() {
  return *cache_replacement;
}

I_CacheReplacement::REPLACEMENT_STRATEGY CacheContainerImpl::get_strategy_id() {
  return strategy_id;
}
std::vector<uint64_t>
CacheContainerImpl::extract_loaded_predicates_from_sequence(
    const std::vector<uint64_t> &input_predicates_ids) {
  std::vector<uint64_t> result;
  auto &predicates_index_cache = pcm->get_predicates_index_cache();

  for (auto current_predicate_id : input_predicates_ids) {
    if (predicates_index_cache.has_predicate_active(current_predicate_id))
      result.push_back(current_predicate_id);
  }

  return result;
}

NodeIdsManager &CacheContainerImpl::get_nodes_ids_manager() { return *nis; }

CacheContainerImpl::CacheContainerImpl(
    std::unique_ptr<PredicatesCacheManager> &&pcm,
    std::unique_ptr<NodeIdsManager> &&nis,
    std::unique_ptr<I_CacheReplacement> &&cache_replacement,
    I_CacheReplacement::REPLACEMENT_STRATEGY strategy_id, bool sort_results)
    : pcm(std::move(pcm)), nis(std::move(nis)),
      cache_replacement(std::move(cache_replacement)), strategy_id(strategy_id),
      sort_results(sort_results) {}
bool CacheContainerImpl::should_sort_results() { return sort_results; }

} // namespace k2cache