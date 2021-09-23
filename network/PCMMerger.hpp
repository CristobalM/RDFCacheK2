//
// Created by cristobal on 9/6/21.
//

#ifndef RDFCACHEK2_PCMMERGER_HPP
#define RDFCACHEK2_PCMMERGER_HPP

#include "I_DataMerger.hpp"
#include <K2TreeMixed.hpp>
#include <PredicatesCacheManager.hpp>

// Wrapper of PredicatesCacheManager merging functionality
// This exists to avoid inheriting PredicatesCacheManager from I_DataMerger
class PCMMerger : public I_DataMerger {
  PredicatesCacheManager &pcm;

public:
  explicit PCMMerger(PredicatesCacheManager &predicates_cache_manager);
  void merge_add_tree(unsigned long predicate_id, K2TreeMixed &k2tree) override;
  void merge_delete_tree(unsigned long predicate_id,
                         K2TreeMixed &k2tree) override;
  void merge_update(std::vector<K2TreeUpdates> &updates) override;
};

#endif // RDFCACHEK2_PCMMERGER_HPP
