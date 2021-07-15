//
// Created by cristobal on 7/14/21.
//

#include "QueryResultIterator.hpp"
QueryResultIterator::QueryResultIterator(
    std::shared_ptr<ResultTableIterator> result_it,
    std::unique_ptr<VarIndexManager> &&vim)
    : result_it(std::move(result_it)), vim(std::move(vim)),
      extra_str_dict(nullptr) {}
QueryResultIterator::QueryResultIterator(
    std::shared_ptr<ResultTableIterator> result_it,
    std::unique_ptr<VarIndexManager> &&vim,
    std::unique_ptr<NaiveDynamicStringDictionary> &&extra_str_dict)
    : result_it(std::move(result_it)), vim(std::move(vim)),
      extra_str_dict(std::move(extra_str_dict)) {}
ResultTableIterator &QueryResultIterator::get_it() { return *result_it; }
VarIndexManager &QueryResultIterator::get_vim() { return *vim; }
NaiveDynamicStringDictionary &QueryResultIterator::get_extra_dict() {
  return *extra_str_dict;
}
bool QueryResultIterator::has_extra_dict() const {
  return extra_str_dict.operator bool();
}
QueryResult QueryResultIterator::as_query_result_original() {
  return QueryResult(result_it->materialize(), std::move(vim),
                     std::move(extra_str_dict));
}
