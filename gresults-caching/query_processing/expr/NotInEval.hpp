//
// Created by cristobal on 6/7/21.
//

#ifndef RDFCACHEK2_NOTINEVAL_HPP
#define RDFCACHEK2_NOTINEVAL_HPP

#include "InEval.hpp"
class NotInEval : public InEval {
public:
  using InEval::InEval;
  bool eval_boolean(const row_t &row) override;
};

#endif // RDFCACHEK2_NOTINEVAL_HPP
