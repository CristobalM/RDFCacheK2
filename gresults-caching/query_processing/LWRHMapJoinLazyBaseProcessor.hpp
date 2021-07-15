//
// Created by cristobal on 7/14/21.
//

#ifndef RDFCACHEK2_LWRHMAPJOINLAZYBASEPROCESSOR_HPP
#define RDFCACHEK2_LWRHMAPJOINLAZYBASEPROCESSOR_HPP

#include "QueryProcHashing.hpp"
#include "ResultTable.hpp"
#include "ResultTableIterator.hpp"
#include "ResultTableIteratorLeftOuterJoin.hpp"
#include "VarIndexManager.hpp"
#include <algorithm>
#include <memory>
#include <set>
#include <unordered_map>
#include <utility>
#include <vector>
class LWRHMapJoinLazyBaseProcessor {
protected:
  using vvul_t = std::vector<std::vector<unsigned long>>;
  using jkey_t = std::vector<unsigned long>;
  using jmap_t = std::unordered_map<jkey_t, vvul_t, fnv_hash_64>;

  std::shared_ptr<ResultTableIterator> left_it;
  std::shared_ptr<ResultTableIterator> right_it;

  std::set<unsigned long> join_vars;
  std::vector<unsigned long> header_values;
  std::unordered_map<unsigned long, unsigned long> header_positions;
  std::vector<unsigned long> join_vars_real_positions;
  jmap_t right_hmap;
  std::vector<unsigned long> left_headers_to_result;
  std::vector<unsigned long> right_values_to_result;

  LWRHMapJoinLazyBaseProcessor(std::shared_ptr<ResultTableIterator> left_it,
                               std::shared_ptr<ResultTableIterator> right_it);
  std::vector<unsigned long> build_header();
  std::unordered_map<unsigned long, unsigned long> build_header_positions();
  std::vector<unsigned long> build_join_vars_real_positions();
  jmap_t build_right_hmap();
  void build_to_result_vec_maps();
  std::set<unsigned long> find_join_vars() const;
  void select_values(
      std::vector<unsigned long> &mutable_keys,
      std::vector<unsigned long> &mutable_values,
      const std::vector<unsigned long> &row,
      const std::vector<unsigned long> &join_var_positions_right,
      const std::vector<unsigned long> &non_join_var_positions_right) const;
  std::pair<std::vector<unsigned long>, std::vector<unsigned long>>
  find_join_nojoin_vars_in_vec(std::vector<unsigned long> &headers_vec);
};

#endif // RDFCACHEK2_LWRHMAPJOINLAZYBASEPROCESSOR_HPP
