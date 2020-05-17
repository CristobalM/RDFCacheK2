//
// Created by Cristobal Miranda, 2020
//

#ifndef RDFCACHEK2_RDFTRIPLE_HPP
#define RDFCACHEK2_RDFTRIPLE_HPP

#include <cstdint>

struct RDFTriple {
  uint64_t subject;
  uint64_t predicate;
  uint64_t object;
};

#endif // RDFCACHEK2_RDFTRIPLE_HPP
