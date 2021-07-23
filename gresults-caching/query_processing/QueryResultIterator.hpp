//
// Created by cristobal on 7/14/21.
//

#ifndef RDFCACHEK2_QUERYRESULTITERATOR_HPP
#define RDFCACHEK2_QUERYRESULTITERATOR_HPP

#include "QueryResult.hpp"
#include "ResultTableIterator.hpp"
#include "VarIndexManager.hpp"
#include <NaiveDynamicStringDictionary.hpp>
#include <memory>
class QueryResultIterator {
  std::shared_ptr<ResultTableIterator> result_it;
  std::shared_ptr<PredicatesCacheManager> cm;
  std::shared_ptr<VarIndexManager> vim;
  std::shared_ptr<NaiveDynamicStringDictionary> extra_str_dict;

public:
  QueryResultIterator(
      std::shared_ptr<ResultTableIterator> result_it,
      std::shared_ptr<PredicatesCacheManager> cm,
      std::shared_ptr<VarIndexManager> vim,
      std::shared_ptr<NaiveDynamicStringDictionary> extra_str_dict);
  QueryResultIterator(std::shared_ptr<ResultTableIterator> result_it,
                      std::shared_ptr<PredicatesCacheManager> cm,
                      std::shared_ptr<VarIndexManager> vim);

  QueryResultIterator(const QueryResultIterator &other) = delete;
  QueryResultIterator &operator=(const QueryResultIterator &other) = delete;
  QueryResultIterator(QueryResultIterator &&other) noexcept;
  QueryResultIterator &operator=(QueryResultIterator &&other) noexcept;

  ResultTableIterator &get_it();
  VarIndexManager &get_vim();
  NaiveDynamicStringDictionary &get_extra_dict();

  bool has_extra_dict() const;
  std::shared_ptr<QueryResult> as_query_result_original();

  std::shared_ptr<VarIndexManager> get_vim_ptr();
  std::shared_ptr<NaiveDynamicStringDictionary> get_extr_str_dict_ptr();

  RDFResource extract_resource(unsigned long id);

  std::shared_ptr<ResultTableIterator> get_it_shared();
};

#endif // RDFCACHEK2_QUERYRESULTITERATOR_HPP
