//
// Created by cristobal on 2/21/22.
//

#ifndef RDFCACHEK2_CACHINGTRIPLETRAVERSALCONDITION_HPP
#define RDFCACHEK2_CACHINGTRIPLETRAVERSALCONDITION_HPP

#include <string>
class CachingTripleTraversalCondition {
  const unsigned long max_points;

  unsigned long points;
public:
  explicit CachingTripleTraversalCondition(unsigned long max_points);
  bool should_cache();
  void set_points(unsigned long new_points);
};

#endif // RDFCACHEK2_CACHINGTRIPLETRAVERSALCONDITION_HPP
