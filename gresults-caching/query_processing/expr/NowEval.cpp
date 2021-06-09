//
// Created by cristobal on 6/4/21.
//

#include "NowEval.hpp"
#include "query_processing/resources/DateTimeResource.hpp"
std::shared_ptr<TermResource>
NowEval::eval_resource(const ExprEval::row_t &row) {
  return std::make_unique<DateTimeResource>(eval_date_time(row));
}
DateInfo NowEval::eval_date_time(const ExprEval::row_t &) {
  return ExprProcessorPersistentData::get().get_now_dateinfo();
}
void NowEval::validate() {
  ExprEval::validate();
  assert_fsize(0);
}
void NowEval::init() { ExprEval::init(); }
