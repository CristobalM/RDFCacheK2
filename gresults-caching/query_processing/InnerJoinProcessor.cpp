//
// Created by cristobal on 15-07-21.
//

#include "InnerJoinProcessor.hpp"
#include "ResultTableIteratorInnerJoin.hpp"
#include <TimeControl.hpp>
InnerJoinProcessor::InnerJoinProcessor(
    std::shared_ptr<ResultTableIterator> left_it,
    std::shared_ptr<ResultTableIterator> right_it, TimeControl &time_control)
    : LWRHMapJoinLazyBaseProcessor(time_control, std::move(left_it),
                                   std::move(right_it)) {}
std::shared_ptr<ResultTableIterator> InnerJoinProcessor::execute_it() {
  return std::make_shared<ResultTableIteratorInnerJoin>(
      std::move(header_values), std::move(join_vars_real_positions), left_it,
      std::move(right_hmap), std::move(left_headers_to_result),
      std::move(right_headers_positions_in_final), time_control);
}
std::shared_ptr<ResultTable> InnerJoinProcessor::execute() {
  return execute_it()->materialize();
}
