//
// Created by cristobal on 5/17/21.
//

#ifndef RDFCACHEK2_PREDICATEFETCHRESULT_HPP
#define RDFCACHEK2_PREDICATEFETCHRESULT_HPP

#include "k2tree/K2TreeMixed.hpp"

namespace k2cache {
class PredicateFetchResult {
  const bool does_exist;
  K2TreeMixed *result;

public:
  PredicateFetchResult(bool does_exist, K2TreeMixed *k2tree);
  const K2TreeMixed &get() const;
  K2TreeMixed &get_mutable();
  bool exists() const;
  bool loaded();
};
} // namespace k2cache

#endif // RDFCACHEK2_PREDICATEFETCHRESULT_HPP
