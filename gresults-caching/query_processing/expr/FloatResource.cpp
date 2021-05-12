//
// Created by cristobal on 4/26/21.
//

#include "FloatResource.hpp"
#include <sstream>
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
