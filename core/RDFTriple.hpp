//
// Created by Cristobal Miranda, 2020
//

#ifndef RDFCACHEK2_RDFTRIPLE_HPP
#define RDFCACHEK2_RDFTRIPLE_HPP

#include <cstdint>

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

struct RDFResource{
  std::string value;
  RDFResourceType resource_type;
  RDFResource(
    std::string &&value,
    RDFResourceType resource_type
  ):
  value(std::move(value)),
  resource_type(resource_type) {}
  RDFResource(
    const std::string &value,
    RDFResourceType resource_type
  ):
  value(value),
  resource_type(resource_type) {}
};

struct RDFResourceReference{
  const std::string &value;
  RDFResourceType resource_type;

  RDFResourceReference(
    const std::string &value,
    RDFResourceType resource_type
  ):
  value(value),
  resource_type(resource_type){}
};

#endif // RDFCACHEK2_RDFTRIPLE_HPP
