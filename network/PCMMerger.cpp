//
// Created by cristobal on 9/6/21.
//

#include "PCMMerger.hpp"

void PCMMerger::merge_add_tree(unsigned long predicate_id,
                               K2TreeMixed &k2tree) {
  pcm.merge_add_tree(predicate_id, k2tree);
}
void PCMMerger::merge_delete_tree(unsigned long predicate_id,
                                  K2TreeMixed &k2tree) {
  pcm.merge_delete_tree(predicate_id, k2tree);
}
PCMMerger::PCMMerger(PredicatesCacheManager &predicates_cache_manager)
    : pcm(predicates_cache_manager) {}
void PCMMerger::merge_update(std::vector<K2TreeUpdates> &updates) {
  pcm.merge_update(updates);
}
