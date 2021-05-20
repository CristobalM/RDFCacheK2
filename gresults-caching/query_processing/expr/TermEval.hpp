//
// Created by cristobal on 4/20/21.
//

#ifndef RDFCACHEK2_TERMEVAL_HPP
#define RDFCACHEK2_TERMEVAL_HPP

#include <unicode/smpdtfmt.h>

#include "ExprEval.hpp"
class TermEval : public ExprEval {
public:
  using ExprEval::ExprEval;
  void validate() override;
  std::shared_ptr<TermResource> eval_resource(const row_t &row) override;
  std::shared_ptr<TermResource> eval_datatype(const row_t &row) override;
  bool eval_boolean(const row_t &row) override;
  int eval_integer(const row_t &row) override;
  float eval_float(const row_t &row) override;
  double eval_double(const row_t &row) override;
  DateInfo eval_date_time(const row_t &row) override;

  void init() override;

private:
  RDFResource eval_variable_get_resource(const row_t &row) const;
  RDFResource eval_term_node(const row_t &row) const;

  bool eval_boolean_from_resource(const RDFResource &resource);
  bool eval_boolean_from_string(const std::string &input_string);
  int eval_integer_from_resource(const RDFResource &resource);
  int eval_integer_from_string(const std::string &basic_string);
  float eval_float_from_resource(const RDFResource &resource);
  float eval_float_from_string(const std::string &basic_string);
  double eval_double_from_resource(const RDFResource &resource);
  double eval_double_from_string(const std::string &basic_string);
  std::shared_ptr<TermResource>
  make_data_type_resource(std::string &&input_string, ExprDataType data_type);
  std::shared_ptr<TermResource> eval_iri_resource(RDFResource &&resource);
  std::shared_ptr<TermResource> create_datatype_resource(RDFResource &&resource,
                                                         bool matches_short);
  bool has_constant_subtree() override;
};

#endif // RDFCACHEK2_TERMEVAL_HPP
