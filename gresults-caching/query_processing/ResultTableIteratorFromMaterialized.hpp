//
// Created by cristobal on 7/14/21.
//

#ifndef RDFCACHEK2_RESULTTABLEITERATORFROMMATERIALIZED_HPP
#define RDFCACHEK2_RESULTTABLEITERATORFROMMATERIALIZED_HPP

#include "ResultTableIterator.hpp"
class ResultTableIteratorFromMaterialized : public ResultTableIterator {
  ResultTable::lvul_t::iterator it;
  std::shared_ptr<ResultTable> table;

public:
  explicit ResultTableIteratorFromMaterialized(
      std::shared_ptr<ResultTable> table);
  bool has_next() override;
  std::vector<unsigned long> next() override;
  std::vector<unsigned long> &get_headers() override;
  void reset_iterator() override;
};

#endif // RDFCACHEK2_RESULTTABLEITERATORFROMMATERIALIZED_HPP
