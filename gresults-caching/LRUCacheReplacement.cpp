#include "LRUCacheReplacement.hpp"

LRUCacheReplacement::LRUCacheReplacement(size_t memory_budget_bytes,
std::shared_ptr<PredicatesCacheManager> &predicates_cache_manager) :
memory_budget_bytes(memory_budget_bytes),
predicates_cache_manager(predicates_cache_manager) {
}
