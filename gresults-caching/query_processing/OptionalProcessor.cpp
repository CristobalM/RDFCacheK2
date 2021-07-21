//
// Created by cristobal on 7/13/21.
//

#include "OptionalProcessor.hpp"
#include "LWRHMapJoinLazyBaseProcessor.hpp"
#include "ResultTableIteratorLeftOuterJoin.hpp"

#include <TimeControl.hpp>
#include <utility>

OptionalProcessor::OptionalProcessor(
    std::shared_ptr<ResultTableIterator> left_it,
    std::shared_ptr<ResultTableIterator> right_it, TimeControl &time_control)
    : LWRHMapJoinLazyBaseProcessor(time_control, std::move(left_it),
                                   std::move(right_it)) {}

std::shared_ptr<ResultTableIterator> OptionalProcessor::execute_it() {
  return std::make_shared<ResultTableIteratorLeftOuterJoin>(
      std::move(header_values), std::move(join_vars_real_positions), left_it,
      std::move(right_hmap), std::move(left_headers_to_result),
      std::move(right_headers_positions_in_final), time_control);
}

std::shared_ptr<ResultTable> OptionalProcessor::execute() {
  return execute_it()->materialize();
}
