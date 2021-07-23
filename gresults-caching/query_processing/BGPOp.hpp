//
// Created by cristobal on 7/12/21.
//

#ifndef RDFCACHEK2_BGPOP_HPP
#define RDFCACHEK2_BGPOP_HPP

#include <K2TreeMixed.hpp>
#include <vector>

class BGPOp {
public:
  enum VARS {
    SUBJECT_VAR = 0,
    OBJECT_VAR = 1,
  };

  struct RunResult {
    bool scan_done;
    bool valid_value;
  };

  // returns true if scanner reached its end
  virtual RunResult run(std::vector<unsigned long> &row_to_fill) = 0;
  virtual void reset_op() = 0;
  virtual K2TreeScanner &get_scanner() = 0;
  virtual ~BGPOp() = default;
};

#endif // RDFCACHEK2_BGPOP_HPP
