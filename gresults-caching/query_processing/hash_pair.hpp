
//
// Created by Cristobal Miranda, 2020
//

#ifndef RDFCACHEK2_HASHPAIR_HPP
#define RDFCACHEK2_HASHPAIR_HPP

#include "KeyPairStr.hpp"

struct hash_pair {
  size_t operator()(const KeyPairStr &p) const;
};

#endif
