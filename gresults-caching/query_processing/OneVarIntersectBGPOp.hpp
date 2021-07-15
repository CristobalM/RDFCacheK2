//
// Created by cristobal on 7/12/21.
//

#ifndef RDFCACHEK2_ONEVARINTERSECTBGPOP_HPP
#define RDFCACHEK2_ONEVARINTERSECTBGPOP_HPP

#include "BGPOp.hpp"
#include <K2TreeMixed.hpp>
#include <memory>
#include <vector>
template <BGPOp::VARS WV> class OneVarIntersectBGPOp : public BGPOp {
  std::unique_ptr<K2TreeMixed::K2TreeScanner> scanner;
  unsigned long var_pos;

public:
  OneVarIntersectBGPOp(std::unique_ptr<K2TreeMixed::K2TreeScanner> &&scanner,
                       unsigned long var_pos);
  RunResult run(std::vector<unsigned long> &row_to_fill) override;
  void reset_op() override;
  K2TreeMixed::K2TreeScanner &get_scanner() override;
};

#endif // RDFCACHEK2_ONEVARINTERSECTBGPOP_HPP
