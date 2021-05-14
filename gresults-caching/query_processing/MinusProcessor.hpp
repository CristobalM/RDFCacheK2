//
// Created by cristobal on 5/14/21.
//

#ifndef RDFCACHEK2_MINUSPROCESSOR_HPP
#define RDFCACHEK2_MINUSPROCESSOR_HPP

#include <memory>

#include <ResultTable.hpp>

class MinusProcessor {
  std::shared_ptr<ResultTable> left_table;
  std::shared_ptr<ResultTable> right_table;

public:
  // expects sorted tables
  MinusProcessor(std::shared_ptr<ResultTable> left_table,
                 std::shared_ptr<ResultTable> right_table);
  std::shared_ptr<ResultTable> execute();
  int compare_rows(ResultTable::vul_t &vector, ResultTable::vul_t &vector_1);
};

#endif // RDFCACHEK2_MINUSPROCESSOR_HPP
