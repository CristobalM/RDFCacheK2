
#include "QueryResult.hpp"

QueryResult::QueryResult(std::shared_ptr<ResultTable> &result_table,
                         std::unique_ptr<VarIndexManager> &&vim)
    : result_table(result_table), vim(std::move(vim)) {}

ResultTable &QueryResult::table() { return *result_table; }

VarIndexManager &QueryResult::get_vim() { return *vim; }
