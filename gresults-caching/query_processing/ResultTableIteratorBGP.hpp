//
// Created by cristobal on 7/11/21.
//

#ifndef RDFCACHEK2_RESULTTABLEITERATORBGP_HPP
#define RDFCACHEK2_RESULTTABLEITERATORBGP_HPP

#include "BGPOp.hpp"
#include "ResultTableIterator.hpp"
#include <K2TreeMixed.hpp>
#include <memory>
#include <set>
#include <vector>
class ResultTableIteratorBGP : public ResultTableIterator {
  std::vector<std::unique_ptr<BGPOp>> bgp_ops;
  std::vector<unsigned long> headers;

  bool next_available;
  std::vector<unsigned long> next_value;

  std::vector<unsigned long> tmp_holder;

  long s_i;

  std::vector<bool> finished_ops;

public:
  ResultTableIteratorBGP(std::vector<std::unique_ptr<BGPOp>> &&bgp_ops,
                         std::vector<unsigned long> &headers);

  bool has_next() override;
  std::vector<unsigned long> next() override;
  void ops_until_last();
  std::vector<unsigned long> &get_headers() override;
  void reset_iterator() override;

private:
  std::vector<unsigned long> next_concrete();
};

#endif // RDFCACHEK2_RESULTTABLEITERATORBGP_HPP
