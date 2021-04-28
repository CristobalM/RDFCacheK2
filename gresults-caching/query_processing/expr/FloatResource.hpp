//
// Created by cristobal on 4/26/21.
//

#ifndef RDFCACHEK2_FLOATRESOURCE_HPP
#define RDFCACHEK2_FLOATRESOURCE_HPP

#include "TermResource.hpp"
class FloatResource : public TermResource {
  float value;

public:
  explicit FloatResource(float value);
  bool is_float() const override;
  bool is_numeric() const override;
  ExprDataType get_datatype() const override;
  int get_integer() const override;
  float get_float() const override;
  double get_double() const override;
  bool operator==(const TermResource &rhs) const override;
};

#endif // RDFCACHEK2_FLOATRESOURCE_HPP
