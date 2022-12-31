//
// Created by cristobal on 05-08-21.
//

#include "NoCachingReplacement.hpp"
namespace k2cache {
bool NoCachingReplacement::hit_key(uint64_t, size_t) { return true; }
void NoCachingReplacement::mark_using(uint64_t) {}
void NoCachingReplacement::mark_ready(uint64_t) {}
std::mutex &NoCachingReplacement::get_replacement_mutex() { return m; }
} // namespace k2cache