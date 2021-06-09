//
// Created by cristobal on 5/31/21.
//

#ifndef RDFCACHEK2_ORDERNODEPROCESSOR_HPP
#define RDFCACHEK2_ORDERNODEPROCESSOR_HPP

#include "VarIndexManager.hpp"
#include <PredicatesCacheManager.hpp>
#include <ResultTable.hpp>
#include <memory>
#include <query_processing/expr/ExprEval.hpp>
#include <sparql_tree.pb.h>
class OrderNodeProcessor {
  std::shared_ptr<ResultTable> current_table;
  const proto_msg::OrderNode &node;

  std::vector<std::unique_ptr<ExprEval>> evaluators;
  const EvalData &eval_data;

public:
  OrderNodeProcessor(std::shared_ptr<ResultTable> input_table,
                     const proto_msg::OrderNode &node,
                     const EvalData &eval_data);
  std::shared_ptr<ResultTable> execute();

private:
  void create_evaluators();
};

#endif // RDFCACHEK2_ORDERNODEPROCESSOR_HPP
