//
// Created by cristobal on 3/2/22.
//

#include "CachedObjectScanner.hpp"
namespace k2cache {
bool CachedObjectScanner::has_next() {
  return current_position < (int)objects.size();
}
std::pair<unsigned long, unsigned long> CachedObjectScanner::next() {
  return {subject_value, objects[current_position++]};
}
CachedObjectScanner::CachedObjectScanner(I_CachedPredicateSource *cached_source,
                                         unsigned long subject_value)
    : objects(cached_source->get_objects(subject_value)), current_position(0),
      subject_value(subject_value) {}
} // namespace k2cache
