//
// Created by cristobal on 4/26/21.
//

#ifndef RDFCACHEK2_DOUBLERESOURCE_HPP
#define RDFCACHEK2_DOUBLERESOURCE_HPP

#include "TermResource.hpp"
class DoubleResource : public TermResource {
  double value;

public:
  explicit DoubleResource(double value);
  bool is_double() const override;
  bool is_numeric() const override;
  ExprDataType get_datatype() const override;
  int get_integer() const override;
  float get_float() const override;
  double get_double() const override;
  bool operator==(const TermResource &rhs) const override;
  bool is_literal() const override;
  RDFResource get_resource_clone() const override;
  int reverse_diff_compare(
      const DoubleResource &double_resource) const override;
  int reverse_diff_compare(
      const IntegerResource &integer_resource) const override;
  int reverse_diff_compare(const FloatResource &float_resource) const override;
  std::shared_ptr<TermResource> cast_to(ExprDataType expr_data_type) override;
};

#endif // RDFCACHEK2_DOUBLERESOURCE_HPP
