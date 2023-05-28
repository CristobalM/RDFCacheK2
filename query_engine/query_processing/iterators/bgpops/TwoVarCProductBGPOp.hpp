//
// Created by cristobal on 7/12/21.
//

#ifndef RDFCACHEK2_TWOVARCPRODUCTBGPOP_HPP
#define RDFCACHEK2_TWOVARCPRODUCTBGPOP_HPP

#include "BGPOp.hpp"
#include <TimeControl.hpp>
#include <memory>
#include <vector>
namespace k2cache {

class TwoVarCProductBGPOp : public BGPOp {

  std::unique_ptr<K2TreeScanner> scanner;
  unsigned long subject_pos;
  unsigned long object_pos;
  TimeControl &time_control;

public:
  TwoVarCProductBGPOp(std::unique_ptr<K2TreeScanner> &&scanner,
                      unsigned long subject_pos, unsigned long object_pos,
                      TimeControl &time_control);
  RunResult run(std::vector<unsigned long> &row_to_fill) override;
  void reset_op() override;
  K2TreeScanner &get_scanner() override;
};
}

#endif // RDFCACHEK2_TWOVARCPRODUCTBGPOP_HPP
