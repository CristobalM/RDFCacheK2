//
// Created by cristobal on 20-07-21.
//

#ifndef RDFCACHEK2_ORDERNODEPROCESSORVEC_HPP
#define RDFCACHEK2_ORDERNODEPROCESSORVEC_HPP

#include "ResultTableVector.hpp"
#include <TimeControl.hpp>
#include <memory>
#include <query_processing/expr/EvalData.hpp>
#include <query_processing/expr/ExprEval.hpp>
#include <sparql_tree.pb.h>

class OrderNodeProcessorVec {
  std::shared_ptr<ResultTableVector> current_table;
  const proto_msg::OrderNode &node;

  std::vector<std::unique_ptr<ExprEval>> evaluators;
  const EvalData &eval_data;
  TimeControl &time_control;

public:
  OrderNodeProcessorVec(std::shared_ptr<ResultTableVector> input_table,
                        const proto_msg::OrderNode &node,
                        const EvalData &eval_data, TimeControl &time_control);
  std::shared_ptr<ResultTableVector> execute();

private:
  void create_evaluators();
};

#endif // RDFCACHEK2_ORDERNODEPROCESSORVEC_HPP
