//
// Created by cristobal on 7/14/21.
//

#include "QueryResultIterator.hpp"
#include "ParsingUtils.hpp"
QueryResultIterator::QueryResultIterator(
    std::shared_ptr<ResultTableIterator> result_it,
    std::shared_ptr<PredicatesCacheManager> cm,
    std::unique_ptr<VarIndexManager> &&vim)
    : result_it(std::move(result_it)), cm(std::move(cm)), vim(std::move(vim)),
      extra_str_dict(nullptr) {}
QueryResultIterator::QueryResultIterator(
    std::shared_ptr<ResultTableIterator> result_it,
    std::shared_ptr<PredicatesCacheManager> cm,
    std::unique_ptr<VarIndexManager> &&vim,
    std::unique_ptr<NaiveDynamicStringDictionary> &&extra_str_dict)
    : result_it(std::move(result_it)), cm(std::move(cm)), vim(std::move(vim)),
      extra_str_dict(std::move(extra_str_dict)) {}
ResultTableIterator &QueryResultIterator::get_it() { return *result_it; }
VarIndexManager &QueryResultIterator::get_vim() { return *vim; }
NaiveDynamicStringDictionary &QueryResultIterator::get_extra_dict() {
  return *extra_str_dict;
}
bool QueryResultIterator::has_extra_dict() const {
  return extra_str_dict.operator bool();
}
std::shared_ptr<QueryResult> QueryResultIterator::as_query_result_original() {
  return std::make_shared<QueryResult>(
      result_it->materialize(), cm, std::move(vim), std::move(extra_str_dict));
}
std::unique_ptr<VarIndexManager> QueryResultIterator::get_vim_ptr() {
  return std::unique_ptr<VarIndexManager>(std::move(vim));
}
std::unique_ptr<NaiveDynamicStringDictionary>
QueryResultIterator::get_extr_str_dict_ptr() {
  return std::unique_ptr<NaiveDynamicStringDictionary>(
      std::move(extra_str_dict));
}
QueryResultIterator::QueryResultIterator(QueryResultIterator &&other) noexcept
    : result_it(std::move(other.result_it)), cm(std::move(other.cm)),
      vim(std::move(other.vim)),
      extra_str_dict(std::move(other.extra_str_dict)) {}

QueryResultIterator &
QueryResultIterator::operator=(QueryResultIterator &&other) noexcept {
  result_it = std::move(other.result_it);
  vim = std::move(other.vim);
  extra_str_dict = std::move(other.extra_str_dict);
  return *this;
}

RDFResource QueryResultIterator::extract_resource(unsigned long id) {
  auto last_id = cm->get_last_id();
  if (id > last_id) {
    return extra_str_dict->extract_resource(id - last_id);
  }
  return cm->extract_resource(id);
}
