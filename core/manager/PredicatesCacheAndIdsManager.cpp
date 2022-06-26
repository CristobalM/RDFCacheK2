//
// Created by cristobal on 26-06-22.
//

#include "PredicatesCacheAndIdsManager.hpp"
namespace k2cache {
PredicatesCacheAndIdsManager::PredicatesCacheAndIdsManager(
    std::unique_ptr<PredicatesCacheManager> &&pcm,
    std::unique_ptr<NodeIdsManager> &&nis)
    : pcm(std::move(pcm)), nis(std::move(nis)) {}

PredicatesCacheManager &PredicatesCacheAndIdsManager::get_pcm() { return *pcm; }
NodeIdsManager &PredicatesCacheAndIdsManager::get_nis() { return *nis; }
} // namespace k2cache
