//
// Created by cristobal on 7/13/21.
//

#ifndef RDFCACHEK2_RESULTTABLEITERATOR_HPP
#define RDFCACHEK2_RESULTTABLEITERATOR_HPP

#include "ResultTable.hpp"
#include <memory>
#include <vector>

class ResultTableIterator {
public:
  virtual bool has_next() = 0;
  virtual std::vector<unsigned long> next() = 0;
  virtual std::vector<unsigned long> &get_headers() = 0;
  virtual void reset_iterator() = 0;
  std::shared_ptr<ResultTable> materialize();

  virtual ~ResultTableIterator() = default;
};
#endif // RDFCACHEK2_RESULTTABLEITERATOR_HPP
