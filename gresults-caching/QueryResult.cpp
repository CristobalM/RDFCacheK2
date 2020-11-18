#include <memory>

#include "QueryResult.hpp"


QueryResult::QueryResult(ResultTable &&result_table)
: result_table(std::move(result_table))
{
}

ResultTable & QueryResult::table(){
  return result_table;
}
