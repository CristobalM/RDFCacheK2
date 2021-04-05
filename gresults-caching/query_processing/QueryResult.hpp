#ifndef _QUERY_RESULT_HPP_
#define _QUERY_RESULT_HPP_

#include <memory>

#include "VarIndexManager.hpp"
#include <ResultTable.hpp>

class QueryResult {
  std::shared_ptr<ResultTable> result_table;
  VarIndexManager vim;

public:
  explicit QueryResult(std::shared_ptr<ResultTable> &result_table,
                       VarIndexManager &&vim);

  ResultTable &table();
  VarIndexManager &get_vim();
};

#endif