//
// Created by cristobal on 20-09-22.
//

#include "NoFIC.hpp"
void k2cache::NoFIC::init_streamer_predicates(
    const std::vector<unsigned long> &) {}
bool k2cache::NoFIC::should_cache(unsigned long) { return false; }
k2cache::FullyIndexedCacheResponse k2cache::NoFIC::get(unsigned long) {
  return FullyIndexedCacheResponse(nullptr);
}
void k2cache::NoFIC::resync_predicate(unsigned long) {}
