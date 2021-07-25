//
// Created by cristobal on 20-07-21.
//

#include "ResultTableIteratorFromMaterializedVector.hpp"
bool ResultTableIteratorFromMaterializedVector::has_next() {
  return current_pos < result_table_vector->data.size();
}

std::vector<unsigned long> ResultTableIteratorFromMaterializedVector::next() {
  return result_table_vector->data[current_pos++];
}
std::vector<unsigned long> &
ResultTableIteratorFromMaterializedVector::get_headers() {
  return result_table_vector->headers;
}
void ResultTableIteratorFromMaterializedVector::reset_iterator() {
  current_pos = 0;
}

ResultTableIteratorFromMaterializedVector::
    ResultTableIteratorFromMaterializedVector(
        std::shared_ptr<ResultTableVector> result_table_vector,
        TimeControl &time_control)
    : ResultTableIterator(time_control),
      result_table_vector(std::move(result_table_vector)), current_pos(0) {}
std::shared_ptr<ResultTableVector>
ResultTableIteratorFromMaterializedVector::materialize_vector() {
  return result_table_vector;
}
