//
// Created by cristobal on 7/14/21.
//

#ifndef RDFCACHEK2_UNIONITERATOR_HPP
#define RDFCACHEK2_UNIONITERATOR_HPP

#include "QueryIterator.hpp"
#include <TimeControl.hpp>
#include <memory>
#include <vector>
class UnionIterator : public QueryIterator {
  std::vector<std::shared_ptr<QueryIterator>> iterators;
  std::vector<unsigned long> headers;
  std::vector<std::vector<unsigned long>> headers_mapping;

  size_t current_it_pos;

public:
  explicit UnionIterator(
      std::vector<std::shared_ptr<QueryIterator>> &&iterators,
      TimeControl &time_control);
  bool has_next() override;
  std::vector<unsigned long> next() override;
  std::vector<unsigned long> &get_headers() override;
  void reset_iterator() override;
  std::vector<unsigned long> next_concrete();
  std::vector<unsigned long> build_headers();
  std::vector<std::vector<unsigned long>> build_headers_mapping();
  std::vector<unsigned long>
  get_mapped_row(std::vector<unsigned long> &input_row, size_t it_pos);
};

#endif // RDFCACHEK2_UNIONITERATOR_HPP
