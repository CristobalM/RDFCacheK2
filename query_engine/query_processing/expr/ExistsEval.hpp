//
// Created by cristobal on 5/18/21.
//

#ifndef RDFCACHEK2_EXISTSEVAL_HPP
#define RDFCACHEK2_EXISTSEVAL_HPP

#include "ExprEval.hpp"
class ExistsEval : public ExprEval {
  bool was_calculated_constant;
  bool is_constant;

public:
  using ExprEval::ExprEval;
  void init() override;
  std::shared_ptr<TermResource> eval_resource(const row_t &row) override;
  bool eval_boolean(const row_t &row) override;
  void validate() override;

private:
  bool has_constant_subtree() override;
  std::set<unsigned long> gather_outside_variables() const;
  std::shared_ptr<VarBindingQProc>
  bind_row_vars_next_eval_data(NaiveDynamicStringDictionary &dictionary,
                               const row_t &row);
};

#endif // RDFCACHEK2_EXISTSEVAL_HPP
