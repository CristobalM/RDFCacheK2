//
// Created by cristobal on 7/14/21.
//

#include "ResultTableIteratorFromMaterialized.hpp"
#include <TimeControl.hpp>
bool ResultTableIteratorFromMaterialized::has_next() {
  return it != table->data.end();
}
std::vector<unsigned long> ResultTableIteratorFromMaterialized::next() {
  auto result = *it;
  ++it;
  return result;
}

std::vector<unsigned long> &ResultTableIteratorFromMaterialized::get_headers() {
  return table->headers;
}
void ResultTableIteratorFromMaterialized::reset_iterator() {
  it = table->data.begin();
}
ResultTableIteratorFromMaterialized::ResultTableIteratorFromMaterialized(
    std::shared_ptr<ResultTable> table, TimeControl &time_control)
    : ResultTableIterator(time_control), it(table->data.begin()),
      table(std::move(table)) {}
std::shared_ptr<ResultTable>
ResultTableIteratorFromMaterialized::materialize() {
  return table;
}
