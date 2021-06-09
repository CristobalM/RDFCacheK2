//
// Created by cristobal on 5/31/21.
//

#include "OrderNodeProcessor.hpp"
#include "ExprProcessor.hpp"
#include <query_processing/utility/StringHandlingUtil.hpp>

std::shared_ptr<ResultTable> OrderNodeProcessor::execute() {

  create_evaluators();

  current_table->data.sort([this](const ResultTable::vul_t &lhs,
                                  const ResultTable::vul_t &rhs) {
    for (int i = 0; i < node.sort_conditions_size(); i++) {
      const auto &sort_condition = node.sort_conditions(i);
      auto lhs_resource = evaluators[i]->eval_resource(lhs);
      auto rhs_resource = evaluators[i]->eval_resource(rhs);
      auto cmp = lhs_resource->diff_compare(*rhs_resource);
      if (cmp != 0) {
        if (sort_condition.direction() == proto_msg::SortDirection::ASCENDING) {
          return cmp < 0;
        } else if (sort_condition.direction() ==
                   proto_msg::SortDirection::DESCENDING) {
          return cmp > 0;
        }
      }
    }
    return false;
  });

  return current_table;
}

OrderNodeProcessor::OrderNodeProcessor(std::shared_ptr<ResultTable> input_table,
                                       const proto_msg::OrderNode &node,
                                       const EvalData &eval_data)
    : current_table(std::move(input_table)), node(node), eval_data(eval_data) {}

void OrderNodeProcessor::create_evaluators() {
  for (int i = 0; i < node.sort_conditions_size(); i++) {
    const auto &sort_condition = node.sort_conditions(i);
    auto evaluator =
        ExprProcessor(eval_data, sort_condition.expr()).create_evaluator();
    evaluator->init();
    evaluators.push_back(std::move(evaluator));
  }
}
