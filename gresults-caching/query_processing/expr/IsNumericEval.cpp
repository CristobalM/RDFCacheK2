//
// Created by cristobal on 4/20/21.
//

#include "IsNumericEval.hpp"
bool IsNumericEval::eval_boolean(const ExprEval::row_t &row) const {
  return false;
}

void IsNumericEval::validate() { ExprEval::validate(); }
void IsNumericEval::init() { ExprEval::init(); }
