//
// Created by cristobal on 5/12/21.
//

#ifndef RDFCACHEK2_INNERJOINPROCESSOR_HPP
#define RDFCACHEK2_INNERJOINPROCESSOR_HPP

#include <memory>
#include <set>
#include <vector>

#include <ResultTable.hpp>

class InnerJoinProcessor {
  std::shared_ptr<ResultTable> left_table;
  std::shared_ptr<ResultTable> right_table;

  // http://www.isthe.com/chongo/tech/comp/fnv/index.html#public_domain, should
  // be used on octects (1byte) instead of on 8 byte numbers
  struct fnv_hash_64 {
    std::size_t operator()(const std::vector<unsigned long> &key) const {
      unsigned long hash = 14695981039346656037UL;
      for (auto value : key) {
        hash *= 1099511628211;
        hash ^= value;
      }
      return static_cast<std::size_t>(hash);
    }
  };

  using vvul_t = std::vector<std::vector<unsigned long>>;
  using jkey_t = std::vector<unsigned long>;
  using jmap_t = std::unordered_map<jkey_t, vvul_t, fnv_hash_64>;

public:
  InnerJoinProcessor(std::shared_ptr<ResultTable> left_table,
                     std::shared_ptr<ResultTable> right_table);
  std::shared_ptr<ResultTable> execute();

private:
  jmap_t
  get_join_map(const std::vector<unsigned long> &join_vars_real_indexes) const;
  std::set<unsigned long> get_join_vars() const;
  void join_values(std::list<std::vector<unsigned long>>::iterator &it,
                   const vvul_t &rows) const;
  void
  select_values(std::vector<unsigned long> &mutable_values,
                const std::vector<unsigned long> &row,
                const std::vector<unsigned long> &join_vars_real_indexes) const;
  std::vector<unsigned long>
  get_real_indexes(const std::vector<unsigned long> &headers,
                   const std::set<unsigned long> &join_vars) const;

  std::vector<unsigned long>
  get_extra_cols(const std::vector<unsigned long> &row,
                 const std::vector<unsigned long> &to_exclude_positions) const;

  std::vector<unsigned long>
  get_diff_list(const std::vector<unsigned long> &all_values,
                const std::set<unsigned long> &values_to_remove) const;
};

#endif // RDFCACHEK2_INNERJOINPROCESSOR_HPP
