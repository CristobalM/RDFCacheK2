//
// Created by cristobal on 7/15/21.
//

#ifndef RDFCACHEK2_RESULTTABLEITERATOREMPTY_HPP
#define RDFCACHEK2_RESULTTABLEITERATOREMPTY_HPP

#include "ResultTableIterator.hpp"
class ResultTableIteratorEmpty : public ResultTableIterator {
  std::vector<unsigned long> headers;

public:
  bool has_next() override;
  std::vector<unsigned long> next() override;
  std::vector<unsigned long> &get_headers() override;
  void reset_iterator() override;

  ResultTableIteratorEmpty(std::vector<unsigned long> headers,
                           TimeControl &time_control);

  explicit ResultTableIteratorEmpty(TimeControl &time_control);
};

#endif // RDFCACHEK2_RESULTTABLEITERATOREMPTY_HPP
