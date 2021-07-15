//
// Created by cristobal on 7/14/21.
//

#ifndef RDFCACHEK2_RESULTTABLEITERATORMINUS_HPP
#define RDFCACHEK2_RESULTTABLEITERATORMINUS_HPP

#include "QueryProcHashing.hpp"
#include "ResultTableIterator.hpp"
#include <memory>
#include <set>
#include <unordered_set>
class ResultTableIteratorMinus : public ResultTableIterator {

  using set_t = std::unordered_set<std::vector<unsigned long>, fnv_hash_64>;

  std::shared_ptr<ResultTableIterator> left_it;
  // std::shared_ptr<ResultTableIterator> right_it;

  set_t right_index;

  bool next_available;
  std::vector<unsigned long> next_result;

  std::vector<unsigned long> key_holder;
  std::vector<unsigned long> key_positions_left;

public:
  bool has_next() override;
  std::vector<unsigned long> next() override;
  std::vector<unsigned long> &get_headers() override;
  void reset_iterator() override;
  ResultTableIteratorMinus(std::shared_ptr<ResultTableIterator> left_it,
                           ResultTableIterator &right_it);

private:
  static ResultTableIteratorMinus::set_t
  build_right_index(ResultTableIterator &right_it);
  std::vector<unsigned long> next_concrete();
  void select_key_values(std::vector<unsigned long> &left_row);
  std::vector<unsigned long>
  build_key_positions_left(std::vector<unsigned long> &right_headers);
};

#endif // RDFCACHEK2_RESULTTABLEITERATORMINUS_HPP
