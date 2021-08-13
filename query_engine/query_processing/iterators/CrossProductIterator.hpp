//
// Created by cristobal on 7/21/21.
//

#ifndef RDFCACHEK2_CROSSPRODUCTITERATOR_HPP
#define RDFCACHEK2_CROSSPRODUCTITERATOR_HPP

#include "QueryIterator.hpp"
#include <TimeControl.hpp>
#include <memory>
class CrossProductIterator : public QueryIterator {
  std::shared_ptr<QueryIterator> left_it;
  std::shared_ptr<QueryIterator> right_it;

  std::vector<unsigned long> headers;

  bool next_available;
  std::vector<unsigned long> next_result;
  std::vector<unsigned long> tmp_holder;

  std::vector<unsigned long> current_left_row;
  bool left_row_active;

public:
  CrossProductIterator(std::shared_ptr<QueryIterator> left_it,
                       std::shared_ptr<QueryIterator> right_it,
                       TimeControl &time_control);
  bool has_next() override;
  std::vector<unsigned long> next() override;
  std::vector<unsigned long> &get_headers() override;
  void reset_iterator() override;
  void build_headers();
  std::vector<unsigned long> next_concrete();
  void map_values_to_holder(std::vector<unsigned long> &right_row);
};

#endif // RDFCACHEK2_CROSSPRODUCTITERATOR_HPP
