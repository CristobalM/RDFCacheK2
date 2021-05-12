//
// Created by cristobal on 4/27/21.
//

#ifndef RDFCACHEK2_STRINGLITERALLANGRESOURCE_HPP
#define RDFCACHEK2_STRINGLITERALLANGRESOURCE_HPP

#include "TermResource.hpp"
class StringLiteralLangResource : public TermResource {
  std::string value;
  std::string lang_tag;

public:
  StringLiteralLangResource(std::string &&value, std::string &&lang_tag);
  bool is_string_literal_lang() const override;
  const std::string &get_literal_lang_string() const override;
  bool operator==(const TermResource &rhs) const override;
  bool contains(TermResource &pattern_resource) const override;
  const std::string &get_lang_tag() const override;
  bool is_literal() const override;
  RDFResource get_resource_clone() const override;
};

#endif // RDFCACHEK2_STRINGLITERALLANGRESOURCE_HPP
