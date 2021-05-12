
#include "QueryResult.hpp"

QueryResult::QueryResult(std::shared_ptr<ResultTable> &result_table,
                         std::unique_ptr<VarIndexManager> &&vim)
    : QueryResult(result_table, std::move(vim), nullptr) {}

ResultTable &QueryResult::table() { return *result_table; }

VarIndexManager &QueryResult::get_vim() { return *vim; }
QueryResult::QueryResult(
    std::shared_ptr<ResultTable> &result_table,
    std::unique_ptr<VarIndexManager> &&vim,
    std::unique_ptr<NaiveDynamicStringDictionary> &&extra_str_dict)
    : result_table(result_table), vim(std::move(vim)),
      extra_str_dict(std::move(extra_str_dict)) {}
bool QueryResult::has_extra_dict() const {
  return extra_str_dict.operator bool();
}
NaiveDynamicStringDictionary &QueryResult::get_extra_dict() {
  return *extra_str_dict;
}
QueryResult::QueryResult() = default;
