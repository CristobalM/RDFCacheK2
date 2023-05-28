//
// Created by cristobal on 7/12/21.
//

#ifndef RDFCACHEK2_ONEVARJOINBGPOP_HPP
#define RDFCACHEK2_ONEVARJOINBGPOP_HPP

#include "BGPOp.hpp"
#include "TimeControl.hpp"
#include <memory>
#include <vector>
namespace k2cache {

template <BGPOp::VARS WV> class OneVarJoinBGPOp : public BGPOp {
  std::unique_ptr<K2TreeScanner> scanner;
  unsigned long var_pos;
  TimeControl &time_control;

public:
  OneVarJoinBGPOp(std::unique_ptr<K2TreeScanner> &&scanner,
                  unsigned long var_pos, TimeControl &time_control);
  RunResult run(std::vector<unsigned long> &row_to_fill) override;
  void reset_op() override;
  K2TreeScanner &get_scanner() override;
};
}

#endif // RDFCACHEK2_ONEVARJOINBGPOP_HPP
