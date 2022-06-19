//
// Created by Cristobal Miranda, 2020
//

#ifndef RDFCACHEK2_CACHE_HPP
#define RDFCACHEK2_CACHE_HPP

#include <memory>
#include <mutex>
#include <string>

#include "manager/PredicatesCacheManager.hpp"
#include "nodeids/NodesSequence.hpp"
#include <I_FileRWHandler.hpp>
#include <replacement/I_CacheReplacement.hpp>
#include <request_msg.pb.h>

#include "CacheArgs.hpp"

class Cache {
  std::shared_ptr<PredicatesCacheManager> cache_manager;
  std::unique_ptr<NodesSequence> nodes_sequence;

  std::unique_ptr<I_CacheReplacement> cache_replacement;

  I_CacheReplacement::REPLACEMENT_STRATEGY strategy_id;

public:
  Cache(std::shared_ptr<PredicatesCacheManager> predicates_cache_manager,
        const CacheArgs &args);

  PredicatesCacheManager &get_pcm();
  NodesSequence &get_nodes_sequence();
  I_CacheReplacement &get_replacement();
  I_CacheReplacement::REPLACEMENT_STRATEGY get_strategy_id();
  std::vector<unsigned long> extract_loaded_predicates_from_sequence(
      const std::vector<unsigned long> &input_predicates_ids);
};

#endif // RDFCACHEK2_CACHE_HPP