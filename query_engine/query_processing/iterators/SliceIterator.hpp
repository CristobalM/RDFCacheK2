//
// Created by cristobal on 15-07-21.
//

#ifndef RDFCACHEK2_SLICEITERATOR_HPP
#define RDFCACHEK2_SLICEITERATOR_HPP

#include "QueryIterator.hpp"
#include <TimeControl.hpp>
class SliceIterator : public QueryIterator {
  std::shared_ptr<QueryIterator> input_it;
  long start;
  long length;
  long current_position;

public:
  SliceIterator(std::shared_ptr<QueryIterator> input_it, long start,
                long length, TimeControl &time_control);
  bool has_next() override;
  std::vector<unsigned long> next() override;
  std::vector<unsigned long> &get_headers() override;
  void reset_iterator() override;
  static long sanitize_length(long value);
  long sanitize_start(long value) const;
};

#endif // RDFCACHEK2_SLICEITERATOR_HPP
