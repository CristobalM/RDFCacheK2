//
// Created by Cristobal Miranda, 2020
//

#ifndef RDFCACHEK2_RDFTRIPLE_HPP
#define RDFCACHEK2_RDFTRIPLE_HPP

#include <cstdint>
#include <memory>
#include <string>

struct RDFTriple {
  uint64_t subject;
  uint64_t predicate;
  uint64_t object;
};

enum RDFResourceType {
  RDF_TYPE_IRI = 0,
  RDF_TYPE_BLANK = 1,
  RDF_TYPE_LITERAL = 2,
};

struct RDFResource {
  std::string value;
  RDFResourceType resource_type;
  RDFResource(std::string &&value, RDFResourceType resource_type)
      : value(std::move(value)), resource_type(resource_type) {}
  RDFResource(const std::string &value, RDFResourceType resource_type)
      : value(value), resource_type(resource_type) {}
};

struct RDFResourceReference {
  const std::string &value;
  RDFResourceType resource_type;

  RDFResourceReference(const std::string &value, RDFResourceType resource_type)
      : value(value), resource_type(resource_type) {}

  RDFResourceReference(const RDFResource &resource)
      : value(resource.value), resource_type(resource.resource_type) {}
};

struct RDFTripleResource {
  RDFResource subject;
  RDFResource predicate;
  RDFResource object;
  RDFTripleResource(RDFResource &&subject, RDFResource &&predicate,
                    RDFResource &&object)
      : subject(std::move(subject)), predicate(std::move(predicate)),
        object(std::move(object)) {}
  RDFTripleResource(RDFResource &subject, RDFResource &predicate,
                    RDFResource &object)
      : subject(subject), predicate(predicate),
        object(object) {}

  
};

struct RDFTripleResourceReference {
  RDFResourceReference subject;
  RDFResourceReference predicate;
  RDFResourceReference object;
  RDFTripleResourceReference(RDFResourceReference subject,
                             RDFResourceReference predicate,
                             RDFResourceReference object)
      : subject(subject), predicate(predicate), object(object) {}

  RDFTripleResourceReference(const RDFTripleResource &rdf_triple_resource)
      : subject(rdf_triple_resource.subject),
        predicate(rdf_triple_resource.predicate),
        object(rdf_triple_resource.object) {}
};

#endif // RDFCACHEK2_RDFTRIPLE_HPP
