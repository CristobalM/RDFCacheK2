//
// Created by cristobal on 25-07-21.
//

#include "ResultTableIteratorOrder.hpp"
bool ResultTableIteratorOrder::has_next() {
  return external_sorted_file_handler.has_next();
}
std::vector<unsigned long> ResultTableIteratorOrder::next() {
  return external_sorted_file_handler.next();
}

std::vector<unsigned long> &ResultTableIteratorOrder::get_headers() {
  return input_it->get_headers();
}

void ResultTableIteratorOrder::reset_iterator() {
  external_sorted_file_handler.reset_iterator();
}

ResultTableIteratorOrder::ResultTableIteratorOrder(
    const std::shared_ptr<ResultTableIterator> &input_it,
    const proto_msg::OrderNode &order_node, EvalData &&eval_data,
    bool remove_duplicates, TimeControl &time_control)
    : ResultTableIterator(time_control), input_it(input_it),
      external_sorted_file_handler(
          input_it, order_node, DEFAULT_MAX_IN_MEMORY_SIZE_SORT, time_control,
          std::move(eval_data), remove_duplicates) {}
