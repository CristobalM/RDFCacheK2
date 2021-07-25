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
  int reverse_diff_compare(const IRIResource &iri_resource) const override;
  int reverse_diff_compare(const StringLiteralLangResource
                               &string_literal_lang_resource) const override;
  int reverse_diff_compare(
      const StringLiteralResource &string_literal_resource) const override;
  int diff_compare(const TermResource &rhs) const override;
  bool can_cast_to_literal_string() const override;
  const std::string &get_literal_string() const override;
  std::string get_content_string_copy() const override;
};

#endif // RDFCACHEK2_STRINGLITERALLANGRESOURCE_HPP
