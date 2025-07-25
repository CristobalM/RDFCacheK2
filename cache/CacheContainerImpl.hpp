//
// Created by Cristobal Miranda, 2020
//

#ifndef RDFCACHEK2_CACHECONTAINERIMPL_HPP
#define RDFCACHEK2_CACHECONTAINERIMPL_HPP

#include <memory>
#include <mutex>
#include <string>

#include "manager/PredicatesCacheManager.hpp"
#include "nodeids/NodesSequence.hpp"
#include <I_FileRWHandler.hpp>
#include <replacement/I_CacheReplacement.hpp>
#include <request_msg.pb.h>

#include "CacheContainer.hpp"

namespace k2cache {
class CacheContainerImpl : public CacheContainer {
  std::unique_ptr<PredicatesCacheManager> pcm;
  std::unique_ptr<NodeIdsManager> nis;
  std::unique_ptr<I_CacheReplacement> cache_replacement;
  I_CacheReplacement::REPLACEMENT_STRATEGY strategy_id;
  bool sort_results;
  int timeout_ms;

public:
  CacheContainerImpl(std::unique_ptr<PredicatesCacheManager> &&pcm,
                     std::unique_ptr<NodeIdsManager> &&nis,
                     std::unique_ptr<I_CacheReplacement> &&cache_replacement,
                     I_CacheReplacement::REPLACEMENT_STRATEGY strategy_id,
                     bool sort_results,
                     int timeout_ms
                     );

  PredicatesCacheManager &get_pcm() override;
  I_CacheReplacement &get_replacement() override;
  I_CacheReplacement::REPLACEMENT_STRATEGY get_strategy_id() override;
  std::vector<uint64_t> extract_loaded_predicates_from_sequence(
      const std::vector<uint64_t> &input_predicates_ids) override;
  NodeIdsManager &get_nodes_ids_manager() override;
  bool should_sort_results() override;
  int get_timeout_ms() override;
};
} // namespace k2cache

#endif // RDFCACHEK2_CACHECONTAINERIMPL_HPP
