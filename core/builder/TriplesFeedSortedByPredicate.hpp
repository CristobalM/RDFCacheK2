//
// Created by Cristobal Miranda on 07-01-23.
//

#ifndef RDFCACHEK2_TRIPLESFEEDSORTEDBYPREDICATE_HPP
#define RDFCACHEK2_TRIPLESFEEDSORTEDBYPREDICATE_HPP

#include <cstdint>

#include "k2tree/RDFTriple.hpp"

namespace k2cache{
struct TriplesFeedSortedByPredicate{
  virtual ~TriplesFeedSortedByPredicate() = default;
  virtual RDFTriple get_next() = 0;
  virtual bool has_next() = 0;
  virtual uint64_t triples_number() = 0;
};
}

#endif //RDFCACHEK2_TRIPLESFEEDSORTEDBYPREDICATE_HPP
