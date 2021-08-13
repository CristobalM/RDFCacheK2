//
// Created by cristobal on 7/11/21.
//

#ifndef RDFCACHEK2_BGPITERATOR_HPP
#define RDFCACHEK2_BGPITERATOR_HPP

#include "QueryIterator.hpp"
#include "bgpops/BGPOp.hpp"
#include <K2TreeMixed.hpp>
#include <TimeControl.hpp>
#include <memory>
#include <set>
#include <vector>
class BGPIterator : public QueryIterator {
  std::vector<std::unique_ptr<BGPOp>> bgp_ops;
  std::vector<unsigned long> headers;

  bool next_available;
  std::vector<unsigned long> next_value;

  std::vector<unsigned long> tmp_holder;

  long s_i;

  std::vector<bool> finished_ops;

public:
  BGPIterator(std::vector<std::unique_ptr<BGPOp>> &&bgp_ops,
              std::vector<unsigned long> &headers, TimeControl &time_control);

  bool has_next() override;
  std::vector<unsigned long> next() override;
  void ops_until_last();
  std::vector<unsigned long> &get_headers() override;
  void reset_iterator() override;

private:
  std::vector<unsigned long> next_concrete();
};

#endif // RDFCACHEK2_BGPITERATOR_HPP
