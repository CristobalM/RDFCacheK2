//
// Created by cristobal on 2/21/22.
//

#include "CachingTripleTraversalCondition.hpp"
CachingTripleTraversalCondition::CachingTripleTraversalCondition(
    unsigned long max_points) : max_points(max_points) {}
bool CachingTripleTraversalCondition::should_cache() {
  return points < max_points;
}
void CachingTripleTraversalCondition::set_points(unsigned long new_points) {
  this->points = new_points;
}
