//
// Created by cristobal on 5/18/21.
//

#include "NotExistsEval.hpp"
void NotExistsEval::init() { ExistsEval::init(); }
std::shared_ptr<TermResource>
NotExistsEval::eval_resource(const ExprEval::row_t &row) {
  // std::cout << "callling not exists eval " << std::endl;
  return generate_from_eval_boolean(row);
}
bool NotExistsEval::eval_boolean(const ExprEval::row_t &row) {
  auto result = !ExistsEval::eval_boolean(row);
  // std::cout << "callling not exists eval (bool): " << result << std::endl;

  return result;
  // return !ExistsEval::eval_boolean(row);
}
void NotExistsEval::validate() { ExistsEval::validate(); }
