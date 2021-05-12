//
// Created by cristobal on 4/26/21.
//

#include "DoubleResource.hpp"

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
