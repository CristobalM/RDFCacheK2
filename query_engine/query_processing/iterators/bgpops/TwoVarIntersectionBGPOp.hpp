//
// Created by cristobal on 7/12/21.
//

#ifndef RDFCACHEK2_TWOVARINTERSECTIONBGPOP_HPP
#define RDFCACHEK2_TWOVARINTERSECTIONBGPOP_HPP

#include "BGPOp.hpp"
#include <K2TreeMixed.hpp>
#include <TimeControl.hpp>
#include <memory>
#include <vector>
class TwoVarIntersectionBGPOp : public BGPOp {
  std::unique_ptr<K2TreeScanner> scanner;
  unsigned long subject_pos;
  unsigned long object_pos;
  TimeControl &time_control;

public:
  TwoVarIntersectionBGPOp(std::unique_ptr<K2TreeScanner> &&scanner,
                          unsigned long subject_pos, unsigned long object_pos,
                          TimeControl &time_control);
  RunResult run(std::vector<unsigned long> &row_to_fill) override;
  void reset_op() override;
  K2TreeScanner &get_scanner() override;
};

#endif // RDFCACHEK2_TWOVARINTERSECTIONBGPOP_HPP
