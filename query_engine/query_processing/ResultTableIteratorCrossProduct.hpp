//
// Created by cristobal on 7/21/21.
//

#ifndef RDFCACHEK2_RESULTTABLEITERATORCROSSPRODUCT_HPP
#define RDFCACHEK2_RESULTTABLEITERATORCROSSPRODUCT_HPP

#include "ResultTableIterator.hpp"
#include <TimeControl.hpp>
#include <memory>
class ResultTableIteratorCrossProduct : public ResultTableIterator {
  std::shared_ptr<ResultTableIterator> left_it;
  std::shared_ptr<ResultTableIterator> right_it;

  std::vector<unsigned long> headers;

  bool next_available;
  std::vector<unsigned long> next_result;
  std::vector<unsigned long> tmp_holder;

  std::vector<unsigned long> current_left_row;
  bool left_row_active;

public:
  ResultTableIteratorCrossProduct(std::shared_ptr<ResultTableIterator> left_it,
                                  std::shared_ptr<ResultTableIterator> right_it,
                                  TimeControl &time_control);
  bool has_next() override;
  std::vector<unsigned long> next() override;
  std::vector<unsigned long> &get_headers() override;
  void reset_iterator() override;
  void build_headers();
  std::vector<unsigned long> next_concrete();
  void map_values_to_holder(std::vector<unsigned long> &right_row);
};

#endif // RDFCACHEK2_RESULTTABLEITERATORCROSSPRODUCT_HPP
