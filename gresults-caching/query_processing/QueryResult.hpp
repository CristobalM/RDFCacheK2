#ifndef _QUERY_RESULT_HPP_
#define _QUERY_RESULT_HPP_

#include <memory>

#include "VarIndexManager.hpp"
#include <NaiveDynamicStringDictionary.hpp>
#include <ResultTable.hpp>

class QueryResult {
  std::shared_ptr<ResultTable> result_table;
  std::unique_ptr<VarIndexManager> vim;
  std::unique_ptr<NaiveDynamicStringDictionary> extra_str_dict;

public:
  QueryResult();
  QueryResult(std::shared_ptr<ResultTable> &result_table,
              std::unique_ptr<VarIndexManager> &&vim);
  QueryResult(std::shared_ptr<ResultTable> &result_table,
              std::unique_ptr<VarIndexManager> &&vim,
              std::unique_ptr<NaiveDynamicStringDictionary> &&extra_str_dict);

  ResultTable &table();
  VarIndexManager &get_vim();
  NaiveDynamicStringDictionary &get_extra_dict();

  bool has_extra_dict() const;
};

#endif