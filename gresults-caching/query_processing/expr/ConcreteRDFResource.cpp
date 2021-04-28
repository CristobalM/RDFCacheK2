//
// Created by cristobal on 4/21/21.
//

#include "ConcreteRDFResource.hpp"
bool ConcreteRDFResource::operator==(const TermResource &rhs) const {
  return rhs.is_concrete() && resource == rhs.get_resource();
}

ConcreteRDFResource::ConcreteRDFResource(RDFResource &&resource)
    : resource(std::move(resource)) {}
ConcreteRDFResource::ConcreteRDFResource(const RDFResource &resource)
    : resource(resource) {}
bool ConcreteRDFResource::is_concrete() const { return true; }
const RDFResource &ConcreteRDFResource::get_resource() const {
  return resource;
}
ConcreteRDFResource ConcreteRDFResource::null_resource() {
  return ConcreteRDFResource(RDFResource::null_resource());
}
std::unique_ptr<TermResource> ConcreteRDFResource::null_resource_ptr() {
  return std::make_unique<ConcreteRDFResource>(RDFResource::null_resource());
}

RDFResource ConcreteRDFResource::get_resource_clone() const { return resource; }
