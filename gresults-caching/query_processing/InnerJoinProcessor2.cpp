//
// Created by cristobal on 15-07-21.
//

#include "InnerJoinProcessor2.hpp"
#include "ResultTableIteratorInnerJoin.hpp"
InnerJoinProcessor2::InnerJoinProcessor2(
    std::shared_ptr<ResultTableIterator> left_it,
    std::shared_ptr<ResultTableIterator> right_it)
    : LWRHMapJoinLazyBaseProcessor(std::move(left_it), std::move(right_it)) {}
std::shared_ptr<ResultTableIterator> InnerJoinProcessor2::execute_it() {
  return std::make_shared<ResultTableIteratorInnerJoin>(
      std::move(header_values), std::move(join_vars_real_positions), left_it,
      std::move(right_hmap), std::move(left_headers_to_result),
      std::move(right_values_to_result));
}
std::shared_ptr<ResultTable> InnerJoinProcessor2::execute() {
  return execute_it()->materialize();
}
