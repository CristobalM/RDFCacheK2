//
// Created by cristobal on 4/26/21.
//

#ifndef RDFCACHEK2_DOUBLERESOURCE_HPP
#define RDFCACHEK2_DOUBLERESOURCE_HPP

#include "TermResource.hpp"
class DoubleResource : public TermResource{
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
};

#endif // RDFCACHEK2_DOUBLERESOURCE_HPP
