//
// Created by cristobal on 6/7/21.
//

#include "RandEval.hpp"
#include "ExprEval.hpp"
void RandEval::init() {
  ExprEval::init();
  uint64_t time_seed =
      std::chrono::high_resolution_clock::now().time_since_epoch().count();
  std::seed_seq ss{uint32_t(time_seed & 0xffffffff), uint32_t(time_seed >> 32)};
  rng.seed(ss);
  uniform_distribution = std::uniform_real_distribution<double>(0, 1);
}
std::shared_ptr<TermResource>
RandEval::eval_resource(const ExprEval::row_t &row) {
  return generate_from_eval_double(row);
}
double RandEval::eval_double(const ExprEval::row_t &) {
  return uniform_distribution(rng);
}

void RandEval::validate() {
  ExprEval::validate();
  assert_fun_size(0);
}
