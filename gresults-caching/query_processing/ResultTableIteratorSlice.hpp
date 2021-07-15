//
// Created by cristobal on 15-07-21.
//

#ifndef RDFCACHEK2_RESULTTABLEITERATORSLICE_HPP
#define RDFCACHEK2_RESULTTABLEITERATORSLICE_HPP

#include "ResultTableIterator.hpp"
class ResultTableIteratorSlice : public ResultTableIterator {
  std::shared_ptr<ResultTableIterator> input_it;
  long start;
  long length;

  long current_position;

public:
  ResultTableIteratorSlice(std::shared_ptr<ResultTableIterator> input_it,
                           long start, long length);
  bool has_next() override;
  std::vector<unsigned long> next() override;
  std::vector<unsigned long> &get_headers() override;
  void reset_iterator() override;
  static long sanitize_length(long value);
  long sanitize_start(long value) const;
};

#endif // RDFCACHEK2_RESULTTABLEITERATORSLICE_HPP
