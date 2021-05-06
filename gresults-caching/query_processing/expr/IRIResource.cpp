//
// Created by cristobal on 5/4/21.
//

#include "IRIResource.hpp"
bool IRIResource::is_iri() const { return true; }
const std::string &IRIResource::get_iri_string() const { return value; }
IRIResource::IRIResource(std::string &&value) : value(std::move(value)) {}
