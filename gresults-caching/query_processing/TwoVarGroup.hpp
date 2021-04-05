//
// Created by Cristobal Miranda, 2020
//

#ifndef RDFCACHEK2_TWOVARGROUP_HPP
#define RDFCACHEK2_TWOVARGROUP_HPP

#include <vector>

#include "KeyPairStr.hpp"
#include "Triple.hpp"

struct TwoVarGroup {
  std::vector<Triple> triples;
  KeyPairStr keys;
};

#endif
