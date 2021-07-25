//
// Created by cristobal on 4/20/21.
//

#ifndef RDFCACHEK2_REGEXEVAL_HPP
#define RDFCACHEK2_REGEXEVAL_HPP

#include "ExprEval.hpp"
class RegexEval : public ExprEval {
public:
  using ExprEval::ExprEval;
  std::shared_ptr<TermResource> eval_resource(const row_t &row) override;
  bool eval_boolean(const row_t &row) override;
  void validate() override;
  void init() override;

private:
  static bool match_pattern(const std::string &input_string,
                            const std::string &pattern);
  static bool match_pattern(const std::string &input_string,
                            const std::string &pattern,
                            const std::string &flags);
};

#endif // RDFCACHEK2_REGEXEVAL_HPP
