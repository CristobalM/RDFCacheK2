#ifndef _QUERY_RESULT_HPP_
#define _QUERY_RESULT_HPP_

#include <ResultTable.hpp>

class QueryResult{
  std::shared_ptr<ResultTable> result_table;

  public:

  explicit QueryResult(std::shared_ptr<ResultTable> &result_table);

  ResultTable &table();
};


#endif 