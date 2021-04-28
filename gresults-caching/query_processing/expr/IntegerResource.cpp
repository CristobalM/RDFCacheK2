//
// Created by cristobal on 4/26/21.
//

#include "IntegerResource.hpp"
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
