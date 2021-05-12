//
// Created by cristobal on 4/21/21.
//

#include "TermResource.hpp"
#include "NullResource.hpp"

ExprDataType TermResource::get_datatype() const {
  // throw std::runtime_error("get_datatype not implemented");
  return EDT_UNKNOWN;
}
bool TermResource::is_concrete() const { return false; }
bool TermResource::is_datatype() const { return false; }
const RDFResource &TermResource::get_resource() const {
  throw std::runtime_error("get_resource not implemented");
}
bool TermResource::is_null() const { return false; }
std::shared_ptr<TermResource> TermResource::null() {
  return std::make_shared<NullResource>();
}
bool TermResource::is_day_time_duration() const { return false; }
bool TermResource::is_string_literal() const { return false; }
bool TermResource::is_integer() const { return false; }
bool TermResource::is_float() const { return false; }
bool TermResource::is_double() const { return false; }
bool TermResource::is_numeric() const { return false; }
bool TermResource::is_boolean() const { return false; }

const std::string &TermResource::get_literal_string() const {
  throw std::runtime_error("get_literal_string not implemented");
}
RDFResource TermResource::get_resource_clone() const {
  throw std::runtime_error("get_resource_clone not implemented");
}
int TermResource::get_integer() const {
  throw std::runtime_error("get_integer not implemented");
}
float TermResource::get_float() const {
  throw std::runtime_error("get_float not implemented");
}
double TermResource::get_double() const {
  throw std::runtime_error("get_double not implemented");
}
bool TermResource::get_boolean() const {
  throw std::runtime_error("get_boolean not implemented");
}
bool TermResource::is_string_literal_lang() const { return false; }
const std::string &TermResource::get_literal_lang_string() const {
  throw std::runtime_error("get_literal_lang_string not implemented");
}
const std::string &TermResource::get_lang_tag() const {
  throw std::runtime_error("get_lang_tag not implemented");
}

bool TermResource::contains(TermResource &) const { return false; }
bool TermResource::is_iri() const { return false; }
const std::string &TermResource::get_iri_string() const {
  throw std::runtime_error("get_iri_string not implemented");
}
bool TermResource::is_literal() const { return false; }
bool TermResource::operator!=(const TermResource &rhs) const {
  return !(*this == rhs);
}
