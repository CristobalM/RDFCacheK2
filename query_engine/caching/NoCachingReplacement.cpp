//
// Created by cristobal on 05-08-21.
//

#include "NoCachingReplacement.hpp"
bool NoCachingReplacement::hit_key(unsigned long, size_t) { return true; }
void NoCachingReplacement::mark_using(unsigned long) {}
void NoCachingReplacement::mark_ready(unsigned long) {}
