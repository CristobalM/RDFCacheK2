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

#include "CacheArgs.hpp"
#include "CacheContainer.hpp"

namespace k2cache {
class CacheContainerImpl : public CacheContainer {
  std::unique_ptr<PredicatesCacheManager> pcm;
  std::unique_ptr<I_CacheReplacement> cache_replacement;
  I_CacheReplacement::REPLACEMENT_STRATEGY strategy_id;

public:
  explicit CacheContainerImpl(const CacheArgs &args);

  PredicatesCacheManager &get_pcm() override;
  I_CacheReplacement &get_replacement() override;
  I_CacheReplacement::REPLACEMENT_STRATEGY get_strategy_id() override;
  std::vector<unsigned long> extract_loaded_predicates_from_sequence(
      const std::vector<unsigned long> &input_predicates_ids) override;
  NodeIdsManager &get_nodes_ids_manager() override;
};
} // namespace k2cache

#endif // RDFCACHEK2_CACHECONTAINERIMPL_HPP
