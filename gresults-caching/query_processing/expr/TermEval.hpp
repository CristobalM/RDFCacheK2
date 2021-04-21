//
// Created by cristobal on 4/20/21.
//

#ifndef RDFCACHEK2_TERMEVAL_HPP
#define RDFCACHEK2_TERMEVAL_HPP

#include "ExprEval.hpp"
class TermEval : public ExprEval {
public:
  [[nodiscard]] RDFResource eval_resource(const row_t &row) const override;
  void validate() override;

private:
  [[nodiscard]] RDFResource eval_variable_get_resource(const row_t &row) const;
};

#endif // RDFCACHEK2_TERMEVAL_HPP
