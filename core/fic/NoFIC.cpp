//
// Created by cristobal on 20-09-22.
//

#include "NoFIC.hpp"

void k2cache::NoFIC::init_streamer_predicates(
    const std::vector<uint64_t> &) {}
bool k2cache::NoFIC::should_cache(uint64_t) {
  return false;
}
k2cache::FullyIndexedCacheResponse k2cache::NoFIC::get(uint64_t) {
  return FullyIndexedCacheResponse(nullptr);
}
void k2cache::NoFIC::resync_predicate(uint64_t) {}
