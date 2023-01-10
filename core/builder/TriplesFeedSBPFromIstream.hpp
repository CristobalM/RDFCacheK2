//
// Created by Cristobal Miranda on 07-01-23.
//

#ifndef RDFCACHEK2_TRIPLESFEEDSBPFROMISTREAM_HPP
#define RDFCACHEK2_TRIPLESFEEDSBPFROMISTREAM_HPP
#include "TriplesFeedSortedByPredicate.hpp"
namespace k2cache{
class TriplesFeedSBPFromIstream : public TriplesFeedSortedByPredicate {
  std::istream &is;
  uint64_t _triples_number;
  uint64_t cnt;
public:
  explicit TriplesFeedSBPFromIstream(std::istream &is);
  RDFTriple get_next() override;
  bool has_next() override;
  uint64_t triples_number() override;
};

}

#endif // RDFCACHEK2_TRIPLESFEEDSBPFROMISTREAM_HPP
