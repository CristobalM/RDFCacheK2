//
// Created by cristobal on 4/20/21.
//

#ifndef RDFCACHEK2_DATATYPEEVAL_HPP
#define RDFCACHEK2_DATATYPEEVAL_HPP

#include "ExprEval.hpp"
class DataTypeEval : public ExprEval {
public:
  using ExprEval::ExprEval;
  std::shared_ptr<TermResource> eval_resource(const row_t &row) override;
  void validate() override;
  void init() override;

private:
  // static RDFResource resource_from_datatype(ExprDataType type);
};

#endif // RDFCACHEK2_DATATYPEEVAL_HPP
