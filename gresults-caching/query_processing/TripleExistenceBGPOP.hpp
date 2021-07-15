//
// Created by cristobal on 7/12/21.
//

#ifndef RDFCACHEK2_TRIPLEEXISTENCEBGPOP_HPP
#define RDFCACHEK2_TRIPLEEXISTENCEBGPOP_HPP

#include "BGPOp.hpp"
#include "Triple.hpp"
#include <K2TreeMixed.hpp>
#include <memory>
class TripleExistenceBGPOP : public BGPOp {
  std::unique_ptr<K2TreeMixed::K2TreeScanner> scanner;
  Triple &triple;

public:
  explicit TripleExistenceBGPOP(
      std::unique_ptr<K2TreeMixed::K2TreeScanner> &&scanner, Triple &triple);
  RunResult run(std::vector<unsigned long> &row_to_fill) override;
  void reset_op() override;
  K2TreeMixed::K2TreeScanner &get_scanner() override;
};

#endif // RDFCACHEK2_TRIPLEEXISTENCEBGPOP_HPP
