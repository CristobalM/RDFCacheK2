//
// Created by cristobal on 6/7/21.
//

#include "NotInEval.hpp"

bool NotInEval::eval_boolean(const ExprEval::row_t &row) {
  return !InEval::eval_boolean(row);
}
