//
// Created by cristobal on 4/26/21.
//

#include "IntegerResource.hpp"

#include "BooleanResource.hpp"
#include "DoubleResource.hpp"
#include "FloatResource.hpp"
#include "StringLiteralResource.hpp"

#include <sstream>
bool IntegerResource::operator==(const TermResource &rhs) const {
  if (rhs.is_integer()) {
    return rhs.get_integer() == value;
  }

  if (rhs.is_float()) {
    return static_cast<float>(value) == rhs.get_float();
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

bool IntegerResource::is_integer() const { return true; }
bool IntegerResource::is_numeric() const { return true; }
ExprDataType IntegerResource::get_datatype() const {
  return ExprDataType::EDT_INTEGER;
}
int IntegerResource::get_integer() const { return value; }
float IntegerResource::get_float() const { return static_cast<float>(value); }
double IntegerResource::get_double() const {
  return static_cast<double>(value);
}
IntegerResource::IntegerResource(int value) : value(value) {}
bool IntegerResource::is_literal() const { return true; }
RDFResource IntegerResource::get_resource_clone() const {
  std::stringstream ss;
  ss << "\"" << std::to_string(value) << "\"^^xsd:integer";
  return RDFResource(ss.str(), RDFResourceType::RDF_TYPE_LITERAL);
}
int IntegerResource::reverse_diff_compare(
    const DoubleResource &double_resource) const {
  auto diff = double_resource.get_double() - get_double();
  if (diff > 0)
    return 1;
  else if (diff < 0)
    return -1;
  return 0;
}
int IntegerResource::reverse_diff_compare(
    const IntegerResource &integer_resource) const {
  auto diff = integer_resource.get_integer() - value;
  if (diff > 0)
    return 1;
  else if (diff < 0)
    return -1;
  return 0;
}
int IntegerResource::reverse_diff_compare(
    const FloatResource &integer_resource) const {
  auto diff = integer_resource.get_float() - get_float();
  if (diff > 0)
    return 1;
  else if (diff < 0)
    return -1;
  return 0;
}
std::shared_ptr<TermResource>
IntegerResource::cast_to(ExprDataType expr_data_type) {
  switch (expr_data_type) {

  case EDT_INTEGER:
    return std::make_shared<IntegerResource>(value);
  case EDT_FLOAT:
    return std::make_shared<FloatResource>((float)value);
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
int IntegerResource::diff_compare(const TermResource &rhs) const {
  return rhs.reverse_diff_compare(*this);
}
bool IntegerResource::can_cast_to_literal_string() const { return true; }
std::string IntegerResource::get_content_string_copy() const {
  return std::to_string(value);
}
