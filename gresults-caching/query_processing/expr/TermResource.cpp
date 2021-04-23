//
// Created by cristobal on 4/21/21.
//

#include "TermResource.hpp"
#include "NullResource.hpp"

ExprDataType TermResource::get_datatype() const {
  throw std::runtime_error("get_datatype not implemented");
}
bool TermResource::is_concrete() const { return false; }
bool TermResource::is_datatype() const { return false; }
const RDFResource &TermResource::get_resource() const {
  throw std::runtime_error("get_resource not implemented");
}
bool TermResource::is_null() const { return false; }
std::unique_ptr<TermResource> TermResource::null() {
  return std::unique_ptr<NullResource>();
}
