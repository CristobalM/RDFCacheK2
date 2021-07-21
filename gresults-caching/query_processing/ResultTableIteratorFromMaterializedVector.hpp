//
// Created by cristobal on 20-07-21.
//

#ifndef RDFCACHEK2_RESULTTABLEITERATORFROMMATERIALIZEDVECTOR_HPP
#define RDFCACHEK2_RESULTTABLEITERATORFROMMATERIALIZEDVECTOR_HPP

#include "ResultTableIterator.hpp"
class ResultTableIteratorFromMaterializedVector : public ResultTableIterator {
  std::shared_ptr<ResultTableVector> result_table_vector;
  size_t current_pos;

public:
  ResultTableIteratorFromMaterializedVector(
      std::shared_ptr<ResultTableVector> result_table_vector,
      TimeControl &time_control);
  bool has_next() override;
  std::vector<unsigned long> next() override;
  std::vector<unsigned long> &get_headers() override;
  void reset_iterator() override;
  std::shared_ptr<ResultTableVector> materialize_vector() override;
};

#endif // RDFCACHEK2_RESULTTABLEITERATORFROMMATERIALIZEDVECTOR_HPP
