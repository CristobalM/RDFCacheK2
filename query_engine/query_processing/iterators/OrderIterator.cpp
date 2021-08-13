//
// Created by cristobal on 25-07-21.
//

#include "OrderIterator.hpp"
bool OrderIterator::has_next() {
  return external_sorted_file_handler.has_next();
}
std::vector<unsigned long> OrderIterator::next() {
  return external_sorted_file_handler.next();
}

std::vector<unsigned long> &OrderIterator::get_headers() {
  return input_it->get_headers();
}

void OrderIterator::reset_iterator() {
  external_sorted_file_handler.reset_iterator();
}

OrderIterator::OrderIterator(const std::shared_ptr<QueryIterator> &input_it,
                             const proto_msg::OrderNode &order_node,
                             EvalData &&eval_data, bool remove_duplicates,
                             TimeControl &time_control)
    : QueryIterator(time_control), input_it(input_it),
      external_sorted_file_handler(
          input_it, order_node, DEFAULT_MAX_IN_MEMORY_SIZE_SORT, time_control,
          std::move(eval_data), remove_duplicates) {}
