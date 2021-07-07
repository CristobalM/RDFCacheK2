//
// Created by cristobal on 5/4/21.
//

#include <cstring>

#include "IRIResource.hpp"
#include "StringLiteralLangResource.hpp"
#include "StringLiteralResource.hpp"

bool IRIResource::is_iri() const { return true; }
const std::string &IRIResource::get_iri_string() const { return value; }
IRIResource::IRIResource(std::string &&value) : value(std::move(value)) {}
int IRIResource::reverse_diff_compare(const IRIResource &iri_resource) const {
  return std::strcmp(iri_resource.get_iri_string().c_str(), value.c_str());
}
int IRIResource::reverse_diff_compare(
    const StringLiteralLangResource &string_literal_lang_resource) const {
  return std::strcmp(
      string_literal_lang_resource.get_literal_lang_string().c_str(),
      value.c_str());
}
int IRIResource::reverse_diff_compare(
    const StringLiteralResource &string_literal_resource) const {
  return std::strcmp(string_literal_resource.get_literal_string().c_str(),
                     value.c_str());
}
int IRIResource::diff_compare(const TermResource &rhs) const {
  return rhs.reverse_diff_compare(*this);
}
