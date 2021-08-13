//
// Created by cristobal on 7/12/21.
//

#ifndef RDFCACHEK2_TWOVARJOINONEBGPOP_HPP
#define RDFCACHEK2_TWOVARJOINONEBGPOP_HPP

#include "BGPOp.hpp"
#include <K2TreeMixed.hpp>
#include <TimeControl.hpp>
#include <memory>
template <BGPOp::VARS WV> class TwoVarJoinOneBGPOp : public BGPOp {

  std::unique_ptr<K2TreeScanner> scanner;
  unsigned long subject_pos;
  unsigned long object_pos;

  std::unique_ptr<K2TreeScanner> current_band_scanner;
  unsigned long current_band;
  K2TreeScanner::BandType current_band_type;
  TimeControl &time_control;

public:
  TwoVarJoinOneBGPOp(std::unique_ptr<K2TreeScanner> &&scanner,
                     unsigned long subject_pos, unsigned long object_pos,
                     TimeControl &time_control);
  RunResult run(std::vector<unsigned long> &row_to_fill) override;
  void reset_op() override;
  K2TreeScanner &get_scanner() override;
};
#endif // RDFCACHEK2_TWOVARJOINONEBGPOP_HPP
