//
// Created by cristobal on 3/2/22.
//

#include "CachedSubjectObjectScanner.hpp"
namespace k2cache {
bool CachedSubjectObjectScanner::has_next() {
  while (current_active_element_position >= active_right_elements->size() &&
         base_set_it != base_set.end()) {
    ++base_set_it;
    current_base_element = *base_set_it;
    active_right_elements = get_active_elements();
    current_active_element_position = 0;
  }
  return base_set_it != base_set.end();
}
std::pair<uint64_t, uint64_t> CachedSubjectObjectScanner::next() {
  auto next_active = active_right_elements->at(current_active_element_position);
  current_active_element_position++;
  if (left_right_dir)
    return {current_base_element, next_active};

  return {next_active, current_base_element};
}
CachedSubjectObjectScanner::CachedSubjectObjectScanner(
    I_CachedPredicateSource *cached_source)
    : cached_source(cached_source),
      left_right_dir(cached_source->get_all_subjects().size() <=
                     cached_source->get_all_objects().size()),
      base_set(left_right_dir ? cached_source->get_all_subjects()
                              : cached_source->get_all_objects()),
      active_right_elements(nullptr), current_active_element_position(0),
      finished(false) {
  base_set_it = base_set.begin();
  current_base_element = *base_set_it;
  active_right_elements = get_active_elements();
}

const std::vector<uint64_t> *
CachedSubjectObjectScanner::get_active_elements() {
  return left_right_dir ? &cached_source->get_objects(current_base_element)
                        : &cached_source->get_subjects(current_base_element);
}
} // namespace k2cache
