//
// Created by cristobal on 7/12/21.
//

#ifndef RDFCACHEK2_ONEVARINTERSECTBGPOP_HPP
#define RDFCACHEK2_ONEVARINTERSECTBGPOP_HPP

#include "BGPOp.hpp"
#include <TimeControl.hpp>
#include <memory>
#include <vector>
namespace k2cache {

template <BGPOp::VARS WV> class OneVarIntersectBGPOp : public BGPOp {
  std::unique_ptr<K2TreeScanner> scanner;
  unsigned long var_pos;
  TimeControl &time_control;

public:
  OneVarIntersectBGPOp(std::unique_ptr<K2TreeScanner> &&scanner,
                       unsigned long var_pos, TimeControl &time_control);
  RunResult run(std::vector<unsigned long> &row_to_fill) override;
  void reset_op() override;
  K2TreeScanner &get_scanner() override;
};
}
#endif // RDFCACHEK2_ONEVARINTERSECTBGPOP_HPP
