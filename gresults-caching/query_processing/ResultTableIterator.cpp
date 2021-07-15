//
// Created by cristobal on 7/13/21.
//
#include <query_processing/ResultTableIterator.hpp>

std::shared_ptr<ResultTable> ResultTableIterator::materialize() {
  auto result = std::make_shared<ResultTable>();
  result->headers = get_headers();
  while (has_next()) {
    result->data.push_back(next());
  }
  return result;
}
