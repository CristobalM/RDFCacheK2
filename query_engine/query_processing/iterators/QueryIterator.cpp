//
// Created by cristobal on 7/13/21.
//
#include "QueryIterator.hpp"
#include <memory>
std::shared_ptr<ResultTable> QueryIterator::materialize() {
  auto result = std::make_shared<ResultTable>();
  if (!time_control.tick())
    return result;
  result->headers = get_headers();
  while (has_next()) {
    result->data.push_back(next());
    if (!time_control.tick())
      return result;
  }
  return result;
}
QueryIterator::QueryIterator(TimeControl &time_control)
    : time_control(time_control) {}
std::shared_ptr<ResultTableVector> QueryIterator::materialize_vector() {
  auto result = std::make_shared<ResultTableVector>();
  if (!time_control.tick())
    return result;
  result->headers = get_headers();
  while (has_next()) {
    result->data.push_back(next());
    if (!time_control.tick())
      return result;
  }
  return result;
}
