//
// Created by cristobal on 19-06-22.
//

#ifndef RDFCACHEK2_POINTSQUERIERIMPL_HPP
#define RDFCACHEK2_POINTSQUERIERIMPL_HPP

#include "PointsQuerier.hpp"
class PointsQuerierImpl : public PointsQuerier {
public:
  bool has(unsigned long subject, unsigned long predicate,
           unsigned long object) override;
};

#endif // RDFCACHEK2_POINTSQUERIERIMPL_HPP
