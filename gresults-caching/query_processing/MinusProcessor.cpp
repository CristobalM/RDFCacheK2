//
// Created by cristobal on 5/14/21.
//

#include "MinusProcessor.hpp"
MinusProcessor::MinusProcessor(std::shared_ptr<ResultTable> left_table,
                               std::shared_ptr<ResultTable> right_table)
    : left_table(std::move(left_table)), right_table(std::move(right_table)) {}

std::shared_ptr<ResultTable> MinusProcessor::execute() {

  auto right_it = right_table->data.begin();
  for (auto it = left_table->data.begin();
       it != left_table->data.end() && right_it != right_table->data.end();) {
    auto next_it = std::next(it);
    auto comparation = compare_rows(*it, *right_it);
    if (comparation == 0) { // l == r
      left_table->data.erase(it);
      it = next_it;
    } else if (comparation > 0) {
      it = next_it;
    } else {
      right_it++;
    }
  }
  return left_table;
}
int MinusProcessor::compare_rows(ResultTable::vul_t &left_row,
                                 ResultTable::vul_t &right_row) {
  auto upper_bound = std::min(left_row.size(), right_row.size());
  for (size_t i = 0; i < upper_bound; i++) {
    auto diff =
        static_cast<long>(right_row[i]) - static_cast<long>(left_row[i]);
    if (diff)
      return diff > 0 ? 1 : -1;
  }
  if (left_row.size() < right_row.size())
    return 1;
  if (left_row.size() > right_row.size())
    return -1;
  return 0;
}
