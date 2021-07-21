//
// Created by cristobal on 7/14/21.
//

#ifndef RDFCACHEK2_RESULTTABLEITERATORLRWHMAPBASE_HPP
#define RDFCACHEK2_RESULTTABLEITERATORLRWHMAPBASE_HPP

#include "QueryProcHashing.hpp"
#include "ResultTableIterator.hpp"
#include <TimeControl.hpp>
#include <set>
class ResultTableIteratorLRWHMapBase : public ResultTableIterator {

protected:
  std::vector<unsigned long> key_holder;
  std::vector<unsigned long> result_holder;
  std::vector<unsigned long> headers;
  std::vector<unsigned long> join_vars_positions;
  std::shared_ptr<ResultTableIterator> left_it;
  std::unordered_map<std::vector<unsigned long>,
                     std::vector<std::vector<unsigned long>>, fnv_hash_64>
      right_hmap;
  std::vector<unsigned long> left_headers_to_result;
  std::vector<unsigned long> right_values_to_result;

  std::vector<unsigned long> current_left_row;
  std::list<std::vector<unsigned long>> current_matched_values;
  std::vector<unsigned long> current_value;

  bool next_available;

  void put_join_values_in_holder(std::vector<unsigned long> &row);
  void put_left_values_in_result(std::vector<unsigned long> &left_values);
  void put_values_in_result(std::vector<unsigned long> &left_values,
                            std::vector<unsigned long> &right_values);

  ResultTableIteratorLRWHMapBase(
      std::vector<unsigned long> &&headers,
      std::vector<unsigned long> &&join_vars_positions,
      std::shared_ptr<ResultTableIterator> left_it,
      std::unordered_map<std::vector<unsigned long>,
                         std::vector<std::vector<unsigned long>>, fnv_hash_64>
          &&right_hmap,
      std::vector<unsigned long> &&left_headers_to_result,
      std::vector<unsigned long> &&right_values_to_result,
      TimeControl &time_control);

  void extract_next();

public:
  bool has_next() override;
  std::vector<unsigned long> &get_headers() override;
  void reset_iterator() override;
};

#endif // RDFCACHEK2_RESULTTABLEITERATORLRWHMAPBASE_HPP
