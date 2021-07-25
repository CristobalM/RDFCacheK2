//
// Created by cristobal on 20-07-21.
//

#include "OrderNodeProcessorVec.hpp"
#include "ExprProcessor.hpp"
#include "quicksort_stoppable.hpp"
OrderNodeProcessorVec::OrderNodeProcessorVec(
    std::shared_ptr<ResultTableVector> input_table,
    const proto_msg::OrderNode &node, const EvalData &eval_data,
    TimeControl &time_control)
    : current_table(std::move(input_table)), node(node), eval_data(eval_data),
      time_control(time_control) {}
std::shared_ptr<ResultTableVector> OrderNodeProcessorVec::execute() {
  create_evaluators();
  inplace_quicksort_stoppable(
      current_table->data,
      [this](const ResultTableVector::vul_t &lhs,
             const ResultTableVector::vul_t &rhs) {
        for (int i = 0; i < node.sort_conditions_size(); i++) {
          const auto &sort_condition = node.sort_conditions(i);
          auto lhs_resource = evaluators[i]->eval_resource(lhs);
          auto rhs_resource = evaluators[i]->eval_resource(rhs);
          auto cmp = lhs_resource->diff_compare(*rhs_resource);

          if (cmp != 0) {
            if (sort_condition.direction() ==
                proto_msg::SortDirection::ASCENDING) {
              return cmp < 0;
            } else if (sort_condition.direction() ==
                       proto_msg::SortDirection::DESCENDING) {
              return cmp > 0;
            }
          }
        }
        return false;
      },
      time_control);

  return current_table;
}
void OrderNodeProcessorVec::create_evaluators() {
  for (int i = 0; i < node.sort_conditions_size(); i++) {
    const auto &sort_condition = node.sort_conditions(i);
    auto evaluator =
        ExprProcessor(eval_data, sort_condition.expr()).create_evaluator();
    evaluator->init();
    evaluators.push_back(std::move(evaluator));
  }
}
