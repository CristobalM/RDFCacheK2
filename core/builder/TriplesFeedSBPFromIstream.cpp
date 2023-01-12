//
// Created by Cristobal Miranda on 07-01-23.
//

#include "TriplesFeedSBPFromIstream.hpp"
#include "serialization_util.hpp"

namespace k2cache {

RDFTriple TriplesFeedSBPFromIstream::get_next() {
  RDFTriple result{};
  result.subject = read_u64(is);
  result.predicate = read_u64(is);
  result.object = read_u64(is);

  ++cnt;

  return result;
}
bool TriplesFeedSBPFromIstream::has_next() { return cnt < _triples_number; }
uint64_t TriplesFeedSBPFromIstream::triples_number() { return _triples_number; }

TriplesFeedSBPFromIstream::TriplesFeedSBPFromIstream(std::istream &is)
    : is(is), _triples_number(read_u64(is)), cnt(0) {}
} // namespace k2cache