//
// Created by cristobal on 15-07-21.
//

#ifndef RDFCACHEK2_RESULTTABLEITERATORINNERJOIN_HPP
#define RDFCACHEK2_RESULTTABLEITERATORINNERJOIN_HPP

#include "ResultTableIteratorLRWHMapBase.hpp"
class ResultTableIteratorInnerJoin : public ResultTableIteratorLRWHMapBase {
public:
  ResultTableIteratorInnerJoin(
      std::vector<unsigned long> &&headers,
      std::vector<unsigned long> &&join_vars_positions,
      std::shared_ptr<ResultTableIterator> left_it,
      std::unordered_map<std::vector<unsigned long>,
                         std::vector<std::vector<unsigned long>>, fnv_hash_64>
          &&right_hmap,
      std::vector<unsigned long> &&left_headers_to_result,
      std::vector<unsigned long> &&right_values_to_result);

  std::vector<unsigned long> next() override;
  std::vector<unsigned long> next_concrete();
};

#endif // RDFCACHEK2_RESULTTABLEITERATORINNERJOIN_HPP
