//
// Created by cristobal on 5/4/21.
//

#ifndef RDFCACHEK2_IRIRESOURCE_HPP
#define RDFCACHEK2_IRIRESOURCE_HPP

#include "TermResource.hpp"
class IRIResource : public TermResource {
  std::string value;

public:
  explicit IRIResource(std::string &&value);
  bool is_iri() const override;
  const std::string &get_iri_string() const override;
  int reverse_diff_compare(const IRIResource &iri_resource) const override;
  int reverse_diff_compare(const StringLiteralLangResource
                               &string_literal_lang_resource) const override;
  int reverse_diff_compare(
      const StringLiteralResource &string_literal_resource) const override;
  int diff_compare(const TermResource &rhs) const override;
};

#endif // RDFCACHEK2_IRIRESOURCE_HPP
