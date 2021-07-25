//
// Created by cristobal on 4/26/21.
//

#include "DoubleResource.hpp"
#include "BooleanResource.hpp"
#include "FloatResource.hpp"
#include "IntegerResource.hpp"
#include "StringLiteralResource.hpp"

#include <sstream>

DoubleResource::DoubleResource(double value) : value(value) {}
bool DoubleResource::is_double() const { return true; }
bool DoubleResource::is_numeric() const { return true; }
ExprDataType DoubleResource::get_datatype() const {
  return ExprDataType::EDT_DOUBLE;
}
int DoubleResource::get_integer() const { return static_cast<int>(value); }
float DoubleResource::get_float() const { return static_cast<float>(value); }
double DoubleResource::get_double() const { return value; }
bool DoubleResource::operator==(const TermResource &rhs) const {
  if (rhs.is_numeric()) {
    return value == rhs.get_double();
  }

  return false;
}
bool DoubleResource::is_literal() const { return true; }
RDFResource DoubleResource::get_resource_clone() const {
  std::stringstream ss;
  ss << "\"" << std::to_string(value) << "\"^^xsd:double";
  return RDFResource(ss.str(), RDFResourceType::RDF_TYPE_LITERAL);
}
int DoubleResource::reverse_diff_compare(
    const DoubleResource &double_resource) const {
  double diff = double_resource.value - value;
  if (diff > 0)
    return 1;
  else if (diff < 0)
    return -1;
  return 0;
}
int DoubleResource::reverse_diff_compare(
    const IntegerResource &integer_resource) const {
  double diff = integer_resource.get_double() - value;
  if (diff > 0)
    return 1;
  else if (diff < 0)
    return -1;
  return 0;
}

int DoubleResource::reverse_diff_compare(
    const FloatResource &float_resource) const {
  double diff = float_resource.get_double() - value;
  if (diff > 0)
    return 1;
  else if (diff < 0)
    return -1;
  return 0;
}
std::shared_ptr<TermResource>
DoubleResource::cast_to(ExprDataType expr_data_type) {
  switch (expr_data_type) {

  case EDT_INTEGER:
    return std::make_shared<IntegerResource>((int)value);
  case EDT_FLOAT:
    return std::make_shared<FloatResource>((float)value);
  case EDT_DECIMAL:
  case EDT_DOUBLE:
    return std::make_shared<DoubleResource>(value);
  case EDT_STRING:
    return std::make_shared<StringLiteralResource>(std::to_string(value),
                                                   EDT_DOUBLE);
  case EDT_BOOLEAN:
    return std::make_shared<BooleanResource>(value != 0);
  case EDT_DATETIME:
  case EDT_UNKNOWN:
  default:
    return TermResource::null();
  }
}
int DoubleResource::diff_compare(const TermResource &rhs) const {
  return rhs.reverse_diff_compare(*this);
}
bool DoubleResource::can_cast_to_literal_string() const { return true; }
std::string DoubleResource::get_content_string_copy() const {
  return std::to_string(value);
}
