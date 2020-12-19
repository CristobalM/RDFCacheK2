
#include "QueryResult.hpp"

QueryResult::QueryResult(std::shared_ptr<ResultTable> &result_table)
    : result_table(result_table) {}

ResultTable &QueryResult::table() { return *result_table; }
