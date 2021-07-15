//
// Created by cristobal on 7/14/21.
//

#ifndef RDFCACHEK2_RESULTTABLEITERATORUNION_HPP
#define RDFCACHEK2_RESULTTABLEITERATORUNION_HPP

#include "ResultTableIterator.hpp"
#include <memory>
#include <vector>
class ResultTableIteratorUnion : public ResultTableIterator {
  std::vector<std::shared_ptr<ResultTableIterator>> iterators;
  std::vector<unsigned long> headers;
  std::vector<std::vector<unsigned long>> headers_mapping;

  size_t current_it_pos;

public:
  explicit ResultTableIteratorUnion(
      std::vector<std::shared_ptr<ResultTableIterator>> &&iterators);
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

#endif // RDFCACHEK2_RESULTTABLEITERATORUNION_HPP
