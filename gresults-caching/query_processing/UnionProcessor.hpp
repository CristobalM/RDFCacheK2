
//
// Created by Cristobal Miranda, 2020
//

#ifndef RDFCACHEK2_UNIONPROCESSOR_HPP
#define RDFCACHEK2_UNIONPROCESSOR_HPP

#include <memory>
#include <set>

#include "ResultTable.hpp"
#include "VarIndexManager.hpp"

class UnionProcessor {

  std::shared_ptr<ResultTable> current_table;

  std::set<unsigned long> current_headers;

public:
  UnionProcessor();

  void combine_table(std::shared_ptr<ResultTable> table);

  std::shared_ptr<ResultTable> get_result();

private:
  void init_headers_set();
  std::set<unsigned long>
  get_headers_set(const std::vector<unsigned long> &headers_list);
  void validate_table(const ResultTable &table);
  void adjust_current_table(const std::set<unsigned long> &extra_headers);

  std::set<unsigned long> select_extra_headers(const ResultTable &table) const;
  std::vector<unsigned long> as_current_table(
      const std::vector<unsigned long> &row,
      const std::vector<unsigned long> &extra_headers_positions,
      const std::vector<unsigned long> &current_table_same_headers_positions,
      const std::vector<unsigned long> &table_same_headers_positions) const;
  std::vector<unsigned long> find_extra_headers_positions(
      const ResultTable &table,
      const std::set<unsigned long> &extra_headers) const;

  std::pair<std::vector<unsigned long>, std::vector<unsigned long>>
  find_same_headers_positions(
      const ResultTable &table,
      const std::set<unsigned long> &extra_headers) const;
};

#endif