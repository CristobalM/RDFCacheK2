//
// Created by cristobal on 4/21/21.
//

#include "TermResource.hpp"
#include "NullResource.hpp"

#include "BooleanResource.hpp"
#include "ConcreteRDFResource.hpp"
#include "DataTypeResource.hpp"
#include "DateTimeResource.hpp"
#include "DayTimeDurationResource.hpp"
#include "DoubleResource.hpp"
#include "FloatResource.hpp"
#include "IRIResource.hpp"
#include "IntegerResource.hpp"
#include "StringLiteralLangResource.hpp"
#include "StringLiteralResource.hpp"

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
std::string TermResource::get_content_string_copy() const {
  throw std::runtime_error("get_content_string_copy not implemented");
}
bool TermResource::is_datetime() const { return false; }
DateInfo TermResource::get_dateinfo() const {
  throw std::runtime_error("get_dateinfo not implemented");
}
int TermResource::diff_compare(const TermResource &) const { return -1; }
int TermResource::reverse_diff_compare(const BooleanResource &) const {
  return -1;
}
int TermResource::reverse_diff_compare(const ConcreteRDFResource &) const {
  return -1;
}
int TermResource::reverse_diff_compare(const DataTypeResource &) const {
  return -1;
}
int TermResource::reverse_diff_compare(const DayTimeDurationResource &) const {
  return -1;
}
int TermResource::reverse_diff_compare(const DoubleResource &) const {
  return -1;
}
int TermResource::reverse_diff_compare(const IntegerResource &) const {
  return -1;
}
int TermResource::reverse_diff_compare(const IRIResource &) const { return -1; }
int TermResource::reverse_diff_compare(
    const StringLiteralLangResource &) const {
  return -1;
}
int TermResource::reverse_diff_compare(const StringLiteralResource &) const {
  return -1;
}
int TermResource::reverse_diff_compare(const FloatResource &) const {
  return -1;
}
int TermResource::reverse_diff_compare(const DateTimeResource &) const {
  return -1;
}
std::shared_ptr<TermResource> TermResource::cast_to(ExprDataType) {
  return null();
}
