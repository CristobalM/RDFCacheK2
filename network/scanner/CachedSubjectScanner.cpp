//
// Created by cristobal on 3/2/22.
//

#include "CachedSubjectScanner.hpp"
namespace k2cache {
CachedSubjectScanner::CachedSubjectScanner(
    I_CachedPredicateSource *cached_source, unsigned long object_value)
    : subjects(cached_source->get_subjects(object_value)), current_position(0),
      object_value(object_value) {}
bool CachedSubjectScanner::has_next() {
  return current_position < (int)subjects.size();
}
std::pair<unsigned long, unsigned long> CachedSubjectScanner::next() {
  return {subjects[current_position++], object_value};
}
} // namespace k2cache
