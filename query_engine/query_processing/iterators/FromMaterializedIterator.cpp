//
// Created by cristobal on 7/14/21.
//

#include "FromMaterializedIterator.hpp"
#include <TimeControl.hpp>
bool FromMaterializedIterator::has_next() { return it != table->data.end(); }
std::vector<unsigned long> FromMaterializedIterator::next() {
  auto result = *it;
  ++it;
  return result;
}

std::vector<unsigned long> &FromMaterializedIterator::get_headers() {
  return table->headers;
}
void FromMaterializedIterator::reset_iterator() { it = table->data.begin(); }
FromMaterializedIterator::FromMaterializedIterator(
    std::shared_ptr<ResultTable> table, TimeControl &time_control)
    : QueryIterator(time_control), it(table->data.begin()),
      table(std::move(table)) {}
std::shared_ptr<ResultTable> FromMaterializedIterator::materialize() {
  return table;
}
