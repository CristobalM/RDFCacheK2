#include "LRUCacheReplacement.hpp"

LRUCacheReplacement::LRUCacheReplacement(size_t memory_budget_bytes,
std::shared_ptr<PredicatesCacheManager> &predicates_cache_manager) :
memory_budget_bytes(memory_budget_bytes),
predicates_cache_manager(predicates_cache_manager),
lru_queue(*this) {
}

void LRUCacheReplacement::retrieve_element(unsigned long element_id) {
  predicates_cache_manager->get_predicates_index_cache().load_single_predicate(element_id);
}

void LRUCacheReplacement::discard_element(unsigned long element_id) {
  predicates_cache_manager->get_predicates_index_cache().discard_in_memory_predicate(element_id);
}

unsigned long LRUCacheReplacement::get_max_size_bytes(){
  return memory_budget_bytes;
}