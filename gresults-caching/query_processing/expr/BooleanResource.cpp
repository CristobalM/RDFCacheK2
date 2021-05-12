//
// Created by cristobal on 4/26/21.
//

#include "BooleanResource.hpp"

#include <sstream>

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
