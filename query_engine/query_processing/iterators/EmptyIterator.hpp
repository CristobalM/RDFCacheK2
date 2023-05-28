//
// Created by cristobal on 7/15/21.
//

#ifndef RDFCACHEK2_EMPTYITERATOR_HPP
#define RDFCACHEK2_EMPTYITERATOR_HPP

#include "QueryIterator.hpp"
namespace k2cache {

class EmptyIterator : public QueryIterator {
  std::vector<unsigned long> headers;

public:
  bool has_next() override;
  std::vector<unsigned long> next() override;
  std::vector<unsigned long> &get_headers() override;
  void reset_iterator() override;

  EmptyIterator(std::vector<unsigned long> headers);

  EmptyIterator() = default;
};
}

#endif // RDFCACHEK2_EMPTYITERATOR_HPP
