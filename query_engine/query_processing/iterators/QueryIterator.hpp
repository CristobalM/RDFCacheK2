//
// Created by cristobal on 7/13/21.
//

#ifndef RDFCACHEK2_QUERYITERATOR_HPP
#define RDFCACHEK2_QUERYITERATOR_HPP

#include <TimeControl.hpp>
#include <memory>
#include <vector>
namespace k2cache {
struct QueryIterator {
  virtual bool has_next() = 0;
  virtual std::vector<unsigned long> next() = 0;
  virtual std::vector<unsigned long> &get_headers() = 0;
  virtual void reset_iterator() = 0;
  virtual ~QueryIterator() = default;
};
}
#endif // RDFCACHEK2_QUERYITERATOR_HPP
