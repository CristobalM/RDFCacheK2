//
// Created by cristobal on 5/18/21.
//

#ifndef RDFCACHEK2_NOTEXISTSEVAL_HPP
#define RDFCACHEK2_NOTEXISTSEVAL_HPP

#include "ExistsEval.hpp"
class NotExistsEval : public ExistsEval {
public:
  using ExistsEval::ExistsEval;
  void init() override;
  std::shared_ptr<TermResource> eval_resource(const row_t &row) override;
  bool eval_boolean(const row_t &row) override;
  void validate() override;
};

#endif // RDFCACHEK2_NOTEXISTSEVAL_HPP
