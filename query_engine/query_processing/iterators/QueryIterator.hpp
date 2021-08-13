//
// Created by cristobal on 7/13/21.
//

#ifndef RDFCACHEK2_QUERYITERATOR_HPP
#define RDFCACHEK2_QUERYITERATOR_HPP

#include "ResultTable.hpp"
#include <TimeControl.hpp>
#include <memory>
#include <query_processing/ResultTableVector.hpp>
#include <vector>

class QueryIterator {
protected:
  TimeControl &time_control;

public:
  explicit QueryIterator(TimeControl &time_control);
  virtual bool has_next() = 0;
  virtual std::vector<unsigned long> next() = 0;
  virtual std::vector<unsigned long> &get_headers() = 0;
  virtual void reset_iterator() = 0;
  virtual std::shared_ptr<ResultTable> materialize();
  virtual std::shared_ptr<ResultTableVector> materialize_vector();

  virtual ~QueryIterator() = default;
};
#endif // RDFCACHEK2_QUERYITERATOR_HPP
