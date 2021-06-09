//
// Created by cristobal on 4/26/21.
//

#include "BooleanResource.hpp"

#include <sstream>

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

bool BooleanResource::operator==(const TermResource &rhs) const {
  if (!rhs.is_boolean())
    return false;
  return value == rhs.get_boolean();
}
bool BooleanResource::is_boolean() const { return true; }
bool BooleanResource::get_boolean() const { return value; }
BooleanResource::BooleanResource(bool value) : value(value) {}
RDFResource BooleanResource::get_resource_clone() const {
  std::stringstream ss;
  ss << "\"" << (value ? "true" : "false") << "\"^^xsd::boolean";
  return RDFResource(ss.str(), RDFResourceType::RDF_TYPE_LITERAL);
}

int BooleanResource::diff_compare(const TermResource &rhs) const {
  return rhs.reverse_diff_compare(*this);
}
int BooleanResource::reverse_diff_compare(
    const BooleanResource &boolean_resource) const {
  if (boolean_resource.get_boolean() == value)
    return 0;
  return -1;
}
std::shared_ptr<TermResource>
BooleanResource::cast_to(ExprDataType expr_data_type) {
  switch (expr_data_type) {
  case EDT_INTEGER:
    return std::make_shared<IntegerResource>(value ? 1 : 0);
  case EDT_DECIMAL:
    return std::make_shared<DoubleResource>(value ? 1.0 : 0);
  case EDT_FLOAT:
    return std::make_shared<FloatResource>(value ? 1.0 : 0);
  case EDT_DOUBLE:
    return std::make_shared<DoubleResource>(value ? 1.0 : 0);
  case EDT_STRING:
    return std::make_shared<StringLiteralResource>(value ? "true" : "false",
                                                   ExprDataType::EDT_BOOLEAN);
  case EDT_BOOLEAN:
    return std::make_shared<BooleanResource>(value);
  case EDT_DATETIME:
  case EDT_UNKNOWN:
  default:
    return TermResource::null();
  }
}
