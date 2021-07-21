//
// Created by cristobal on 7/14/21.
//

#ifndef RDFCACHEK2_RESULTTABLEITERATORLEFTOUTERJOIN_HPP
#define RDFCACHEK2_RESULTTABLEITERATORLEFTOUTERJOIN_HPP

#include "QueryProcHashing.hpp"
#include "ResultTableIterator.hpp"
#include "ResultTableIteratorLRWHMapBase.hpp"
#include <TimeControl.hpp>
#include <set>
class ResultTableIteratorLeftOuterJoin : public ResultTableIteratorLRWHMapBase {

public:
  ResultTableIteratorLeftOuterJoin(
      std::vector<unsigned long> &&headers,
      std::vector<unsigned long> &&join_vars_positions,
      std::shared_ptr<ResultTableIterator> left_it,
      std::unordered_map<std::vector<unsigned long>,
                         std::vector<std::vector<unsigned long>>, fnv_hash_64>
          &&right_hmap,
      std::vector<unsigned long> &&left_headers_to_result,
      std::vector<unsigned long> &&right_values_to_result,
      TimeControl &time_control);
  std::vector<unsigned long> next() override;

private:
  std::vector<unsigned long> next_concrete();
};

#endif // RDFCACHEK2_RESULTTABLEITERATORLEFTOUTERJOIN_HPP
