//
// Created by cristobal on 7/14/21.
//

#ifndef RDFCACHEK2_QUERYRESULTITERATORHOLDER_HPP
#define RDFCACHEK2_QUERYRESULTITERATORHOLDER_HPP

#include "QueryResult.hpp"
#include "VarIndexManager.hpp"
#include "iterators/QueryIterator.hpp"
#include <NaiveDynamicStringDictionary.hpp>
#include <memory>
class QueryResultIteratorHolder {
  std::shared_ptr<QueryIterator> result_it;
  std::shared_ptr<PredicatesCacheManager> cm;
  std::shared_ptr<VarIndexManager> vim;
  std::shared_ptr<NaiveDynamicStringDictionary> extra_str_dict;

public:
  QueryResultIteratorHolder(
      std::shared_ptr<QueryIterator> result_it,
      std::shared_ptr<PredicatesCacheManager> cm,
      std::shared_ptr<VarIndexManager> vim,
      std::shared_ptr<NaiveDynamicStringDictionary> extra_str_dict);
  QueryResultIteratorHolder(std::shared_ptr<QueryIterator> result_it,
                            std::shared_ptr<PredicatesCacheManager> cm,
                            std::shared_ptr<VarIndexManager> vim);

  QueryResultIteratorHolder(const QueryResultIteratorHolder &other) = delete;
  QueryResultIteratorHolder &
  operator=(const QueryResultIteratorHolder &other) = delete;
  QueryResultIteratorHolder(QueryResultIteratorHolder &&other) noexcept;
  QueryResultIteratorHolder &
  operator=(QueryResultIteratorHolder &&other) noexcept;

  QueryIterator &get_it();
  VarIndexManager &get_vim();
  NaiveDynamicStringDictionary &get_extra_dict();

  bool has_extra_dict() const;
  std::shared_ptr<QueryResult> as_query_result_original();

  std::shared_ptr<VarIndexManager> get_vim_ptr();
  std::shared_ptr<NaiveDynamicStringDictionary> get_extr_str_dict_ptr();

  RDFResource extract_resource(unsigned long id);

  std::shared_ptr<QueryIterator> get_it_shared();
};

#endif // RDFCACHEK2_QUERYRESULTITERATORHOLDER_HPP
