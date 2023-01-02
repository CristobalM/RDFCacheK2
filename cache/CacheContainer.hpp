//
// Created by cristobal on 26-06-22.
//

#ifndef RDFCACHEK2_CACHECONTAINER_HPP
#define RDFCACHEK2_CACHECONTAINER_HPP
#include "manager/PredicatesCacheManager.hpp"
#include "nodeids/NodeIdsManager.hpp"
#include "nodeids/NodesSequence.hpp"
#include "replacement/I_CacheReplacement.hpp"

namespace k2cache {
struct CacheContainer {
  virtual ~CacheContainer() = default;
  virtual I_CacheReplacement &get_replacement() = 0;
  virtual PredicatesCacheManager &get_pcm() = 0;
  virtual I_CacheReplacement::REPLACEMENT_STRATEGY get_strategy_id() = 0;
  virtual std::vector<uint64_t> extract_loaded_predicates_from_sequence(
      const std::vector<uint64_t> &input_predicates_ids) = 0;
  virtual NodeIdsManager &get_nodes_ids_manager() = 0;
  virtual bool should_sort_results() = 0;
};
} // namespace k2cache

#endif // RDFCACHEK2_CACHECONTAINER_HPP
