//
// Created by cristobal on 7/14/21.
//

#ifndef RDFCACHEK2_RESULTTABLEITERATORPROJECT_HPP
#define RDFCACHEK2_RESULTTABLEITERATORPROJECT_HPP

#include "ResultTableIterator.hpp"
#include <set>
class ResultTableIteratorProject : public ResultTableIterator {
  std::shared_ptr<ResultTableIterator> input_it;
  std::vector<unsigned long> vars_to_keep_position;
  std::vector<unsigned long> headers;
  std::vector<unsigned long> tmp_holder;

public:
  ResultTableIteratorProject(std::shared_ptr<ResultTableIterator> input_it,
                             std::set<unsigned long> &vars_to_keep);
  bool has_next() override;
  std::vector<unsigned long> next() override;
  std::vector<unsigned long> &get_headers() override;
  void reset_iterator() override;
  std::vector<unsigned long>
  find_vars_to_keep_position(std::set<unsigned long> &vars_to_keep);
  std::vector<unsigned long> build_headers();
};

#endif // RDFCACHEK2_RESULTTABLEITERATORPROJECT_HPP
