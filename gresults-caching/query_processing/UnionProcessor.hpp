
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

  // VarIndexManager &vim;

  std::shared_ptr<ResultTable> current_table;

  std::set<unsigned long> current_headers;

public:
  // UnionProcessor(VarIndexManager &vim);
  UnionProcessor();

  // Doesn't delete duplicates
  void combine_table(std::shared_ptr<ResultTable> table);

  std::shared_ptr<ResultTable> get_result();

private:
  void init_headers_set();
  std::set<unsigned long>
  get_headers_set(const std::vector<unsigned long> &headers_list);
  void validate_table(const ResultTable &table);
  std::vector<unsigned long> find_table_permutation(const ResultTable &table);
  std::vector<unsigned long>
  permutate_row(const std::vector<unsigned long> &table_permutation,
                std::vector<unsigned long> &input_row);
};

#endif