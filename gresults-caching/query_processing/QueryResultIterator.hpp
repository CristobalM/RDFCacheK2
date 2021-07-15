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
  std::unique_ptr<VarIndexManager> vim;
  std::unique_ptr<NaiveDynamicStringDictionary> extra_str_dict;

public:
  QueryResultIterator(
      std::shared_ptr<ResultTableIterator> result_it,
      std::unique_ptr<VarIndexManager> &&vim,
      std::unique_ptr<NaiveDynamicStringDictionary> &&extra_str_dict);
  QueryResultIterator(std::shared_ptr<ResultTableIterator> result_it,
                      std::unique_ptr<VarIndexManager> &&vim);

  ResultTableIterator &get_it();
  VarIndexManager &get_vim();
  NaiveDynamicStringDictionary &get_extra_dict();

  bool has_extra_dict() const;
  QueryResult as_query_result_original();
};

#endif // RDFCACHEK2_QUERYRESULTITERATOR_HPP
