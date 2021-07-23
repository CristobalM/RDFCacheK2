
#include "QueryResult.hpp"

QueryResult::QueryResult(std::shared_ptr<ResultTable> &result_table,
                         std::shared_ptr<PredicatesCacheManager> cm,
                         std::shared_ptr<VarIndexManager> vim)
    : QueryResult(result_table, std::move(cm), std::move(vim), nullptr) {}

ResultTable &QueryResult::table() { return *result_table; }

VarIndexManager &QueryResult::get_vim() { return *vim; }
QueryResult::QueryResult(
    std::shared_ptr<ResultTable> result_table,
    std::shared_ptr<PredicatesCacheManager> cm,
    std::shared_ptr<VarIndexManager> vim,
    std::shared_ptr<NaiveDynamicStringDictionary> extra_str_dict)
    : result_table(std::move(result_table)), cm(std::move(cm)),
      vim(std::move(vim)), extra_str_dict(std::move(extra_str_dict)) {}
bool QueryResult::has_extra_dict() const {
  return extra_str_dict.operator bool();
}
NaiveDynamicStringDictionary &QueryResult::get_extra_dict() {
  return *extra_str_dict;
}
RDFResource QueryResult::extract_resource(unsigned long id) {
  auto last_id = cm->get_last_id();
  if (id > last_id) {
    return extra_str_dict->extract_resource(id - last_id);
  }
  return cm->extract_resource(id);
}
QueryResult::QueryResult() = default;
