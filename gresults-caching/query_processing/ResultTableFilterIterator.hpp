//
// Created by cristobal on 7/14/21.
//

#ifndef RDFCACHEK2_RESULTTABLEFILTERITERATOR_HPP
#define RDFCACHEK2_RESULTTABLEFILTERITERATOR_HPP

#include "ResultTableIterator.hpp"
#include "VarIndexManager.hpp"
#include <PredicatesCacheManager.hpp>
#include <memory>
#include <query_processing/expr/ExprEval.hpp>
#include <vector>
class ResultTableFilterIterator : public ResultTableIterator {
  std::shared_ptr<ResultTableIterator> input_it;
  std::vector<unsigned long> next_row;
  bool next_available;
  std::shared_ptr<std::unordered_map<std::string, unsigned long>>
      var_pos_mapping;
  std::vector<std::unique_ptr<ExprEval>> bool_expressions;
  EvalData eval_data;

public:
  ResultTableFilterIterator(
      std::shared_ptr<ResultTableIterator> input_it, VarIndexManager &vim,
      std::vector<const proto_msg::ExprNode *> &expr_nodes,
      const PredicatesCacheManager &cm,
      NaiveDynamicStringDictionary &extra_str_dict);
  bool has_next() override;
  std::vector<unsigned long> next() override;
  std::vector<unsigned long> &get_headers() override;
  void reset_iterator() override;

private:
  std::vector<unsigned long> next_concrete();

  std::shared_ptr<std::unordered_map<std::string, unsigned long>>
  get_var_pos_mapping(VarIndexManager &vim);
};

#endif // RDFCACHEK2_RESULTTABLEFILTERITERATOR_HPP
