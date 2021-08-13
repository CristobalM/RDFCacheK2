//
// Created by cristobal on 7/14/21.
//

#include "QueryResultIteratorHolder.hpp"
#include "ParsingUtils.hpp"
QueryResultIteratorHolder::QueryResultIteratorHolder(
    std::shared_ptr<QueryIterator> result_it,
    std::shared_ptr<PredicatesCacheManager> cm,
    std::shared_ptr<VarIndexManager> vim)
    : result_it(std::move(result_it)), cm(std::move(cm)), vim(std::move(vim)),
      extra_str_dict(nullptr) {}
QueryResultIteratorHolder::QueryResultIteratorHolder(
    std::shared_ptr<QueryIterator> result_it,
    std::shared_ptr<PredicatesCacheManager> cm,
    std::shared_ptr<VarIndexManager> vim,
    std::shared_ptr<NaiveDynamicStringDictionary> extra_str_dict)
    : result_it(std::move(result_it)), cm(std::move(cm)), vim(std::move(vim)),
      extra_str_dict(std::move(extra_str_dict)) {}
QueryIterator &QueryResultIteratorHolder::get_it() { return *result_it; }
VarIndexManager &QueryResultIteratorHolder::get_vim() { return *vim; }
NaiveDynamicStringDictionary &QueryResultIteratorHolder::get_extra_dict() {
  return *extra_str_dict;
}
bool QueryResultIteratorHolder::has_extra_dict() const {
  return extra_str_dict.operator bool();
}
std::shared_ptr<QueryResult>
QueryResultIteratorHolder::as_query_result_original() {
  return std::make_shared<QueryResult>(result_it->materialize(), cm, vim,
                                       extra_str_dict);
}
std::shared_ptr<VarIndexManager> QueryResultIteratorHolder::get_vim_ptr() {
  return vim;
}
std::shared_ptr<NaiveDynamicStringDictionary>
QueryResultIteratorHolder::get_extr_str_dict_ptr() {
  return extra_str_dict;
}
QueryResultIteratorHolder::QueryResultIteratorHolder(
    QueryResultIteratorHolder &&other) noexcept
    : result_it(std::move(other.result_it)), cm(std::move(other.cm)),
      vim(std::move(other.vim)),
      extra_str_dict(std::move(other.extra_str_dict)) {}

QueryResultIteratorHolder &QueryResultIteratorHolder::operator=(
    QueryResultIteratorHolder &&other) noexcept {
  result_it = std::move(other.result_it);
  vim = std::move(other.vim);
  extra_str_dict = std::move(other.extra_str_dict);
  return *this;
}

RDFResource QueryResultIteratorHolder::extract_resource(unsigned long id) {
  auto last_id = cm->get_last_id();
  if (id > last_id) {
    return extra_str_dict->extract_resource(id - last_id);
  }
  return cm->extract_resource(id);
}
std::shared_ptr<QueryIterator> QueryResultIteratorHolder::get_it_shared() {
  return result_it;
}
