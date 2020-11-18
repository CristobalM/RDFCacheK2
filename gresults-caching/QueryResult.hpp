#ifndef _QUERY_RESULT_HPP_
#define _QUERY_RESULT_HPP_

#include <ResultTable.hpp>

class QueryResult{
  ResultTable result_table;

  public:

  explicit QueryResult(ResultTable &&result_table);

  ResultTable &table();
};


#endif 