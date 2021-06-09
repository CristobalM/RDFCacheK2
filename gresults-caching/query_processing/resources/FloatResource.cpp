//
// Created by cristobal on 4/26/21.
//
#include <sstream>

#include "BooleanResource.hpp"
#include "DoubleResource.hpp"
#include "FloatResource.hpp"
#include "IntegerResource.hpp"
#include "StringLiteralResource.hpp"

bool FloatResource::is_float() const { return true; }
bool FloatResource::is_numeric() const { return true; }
ExprDataType FloatResource::get_datatype() const {
  return ExprDataType::EDT_FLOAT;
}
int FloatResource::get_integer() const { return static_cast<int>(value); }
float FloatResource::get_float() const { return value; }
double FloatResource::get_double() const { return static_cast<double>(value); }
FloatResource::FloatResource(float value) : value(value) {}

bool FloatResource::operator==(const TermResource &rhs) const {
  if (rhs.is_integer() || rhs.is_float()) {
    return rhs.get_float() == value;
  }

  if (rhs.is_double()) {
    return static_cast<double>(value) == rhs.get_double();
  }

  if (rhs.is_numeric()) {
    // never should happen but to cover any number as a double
    return static_cast<double>(value) == rhs.get_double();
  }

  return false;
}
bool FloatResource::is_literal() const { return true; }
RDFResource FloatResource::get_resource_clone() const {
  std::stringstream ss;
  ss << "\"" << std::to_string(value) << "\"^^xsd:float";
  return RDFResource(ss.str(), RDFResourceType::RDF_TYPE_LITERAL);
}
int FloatResource::reverse_diff_compare(
    const DoubleResource &double_resource) const {
  auto diff = double_resource.get_double() - get_double();
  if (diff > 0)
    return 1;
  else if (diff < 0)
    return -1;
  return 0;
}
int FloatResource::reverse_diff_compare(
    const IntegerResource &integer_resource) const {
  auto diff = integer_resource.get_float() - value;
  if (diff > 0)
    return 1;
  else if (diff < 0)
    return -1;
  return 0;
}

int FloatResource::reverse_diff_compare(
    const FloatResource &float_resource) const {
  auto diff = float_resource.value - value;
  if (diff > 0)
    return 1;
  else if (diff < 0)
    return -1;
  return 0;
}
std::shared_ptr<TermResource>
FloatResource::cast_to(ExprDataType expr_data_type) {
  switch (expr_data_type) {

  case EDT_INTEGER:
    return std::make_shared<IntegerResource>((int)value);
  case EDT_FLOAT:
    return std::make_shared<FloatResource>(value);
  case EDT_DECIMAL:
  case EDT_DOUBLE:
    return std::make_shared<DoubleResource>((double)value);
  case EDT_STRING:
    return std::make_shared<StringLiteralResource>(std::to_string(value),
                                                   EDT_STRING);
  case EDT_BOOLEAN:
    return std::make_shared<BooleanResource>(value != 0);
  case EDT_DATETIME:
  case EDT_UNKNOWN:
  default:
    return TermResource::null();
  }
}
