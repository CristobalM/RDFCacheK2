//
// Created by cristobal on 4/21/21.
//

#ifndef RDFCACHEK2_CONCRETERDFRESOURCE_HPP
#define RDFCACHEK2_CONCRETERDFRESOURCE_HPP

#include "TermResource.hpp"
#include <RDFTriple.hpp>
class ConcreteRDFResource : public TermResource {
  RDFResource resource;

public:
  ConcreteRDFResource(RDFResource &&resource);
  ConcreteRDFResource(const RDFResource &resource);
  bool operator==(const TermResource &rhs) const override;
  bool is_concrete() const override;
  const RDFResource &get_resource() const override;

  static ConcreteRDFResource null_resource();
  static std::shared_ptr<TermResource> null_resource_ptr();
  RDFResource get_resource_clone() const override;
  bool contains(TermResource &pattern_resource) const override;
  bool is_literal() const override;
  const std::string &get_literal_string() const override;
  std::string get_content_string_copy() const override;
};

#endif // RDFCACHEK2_CONCRETERDFRESOURCE_HPP
