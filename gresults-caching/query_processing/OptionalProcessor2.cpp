//
// Created by cristobal on 7/13/21.
//

#include "OptionalProcessor2.hpp"
#include "LWRHMapJoinLazyBaseProcessor.hpp"
#include "ResultTableIteratorLeftOuterJoin.hpp"

#include <utility>

OptionalProcessor2::OptionalProcessor2(
    std::shared_ptr<ResultTableIterator> left_it,
    std::shared_ptr<ResultTableIterator> right_it)
    : LWRHMapJoinLazyBaseProcessor(std::move(left_it), std::move(right_it)) {}

std::shared_ptr<ResultTableIterator> OptionalProcessor2::execute_it() {
  return std::make_shared<ResultTableIteratorLeftOuterJoin>(
      std::move(header_values), std::move(join_vars_real_positions), left_it,
      std::move(right_hmap), std::move(left_headers_to_result),
      std::move(right_values_to_result));
}

std::shared_ptr<ResultTable> OptionalProcessor2::execute() {
  return execute_it()->materialize();
}
