//
// Created by cristobal on 4/26/21.
//

#ifndef RDFCACHEK2_INTEGERRESOURCE_HPP
#define RDFCACHEK2_INTEGERRESOURCE_HPP

#include "TermResource.hpp"
class IntegerResource : public TermResource {
  int value;

public:
  explicit IntegerResource(int value);
  bool operator==(const TermResource &rhs) const override;
  bool is_integer() const override;
  bool is_numeric() const override;
  ExprDataType get_datatype() const override;
  int get_integer() const override;
  float get_float() const override;
  double get_double() const override;
  bool is_literal() const override;
  RDFResource get_resource_clone() const override;
};

#endif // RDFCACHEK2_INTEGERRESOURCE_HPP
