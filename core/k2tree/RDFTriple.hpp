//
// Created by Cristobal Miranda, 2020
//

#ifndef RDFCACHEK2_RDFTRIPLE_HPP
#define RDFCACHEK2_RDFTRIPLE_HPP

#include <cstdint>
#include <memory>
#include <string>

#include "request_msg.pb.h"

struct RDFTriple {
  uint64_t subject;
  uint64_t predicate;
  uint64_t object;
};

enum RDFResourceType {
  RDF_TYPE_IRI = 0,
  RDF_TYPE_BLANK = 1,
  RDF_TYPE_LITERAL = 2,
  NULL_RESOURCE_TYPE,
};

struct RDFResource {
  std::string value;
  RDFResourceType resource_type;
  RDFResource() : resource_type(NULL_RESOURCE_TYPE) {}
  RDFResource(std::string &&value, RDFResourceType resource_type)
      : value(std::move(value)), resource_type(resource_type) {}
  RDFResource(const std::string &value, RDFResourceType resource_type)
      : value(value), resource_type(resource_type) {}
  RDFResource(const char *value, RDFResourceType resource_type)
      : value(value), resource_type(resource_type) {}

  RDFResource(const RDFResource &other)
      : value(other.value), resource_type(other.resource_type) {}

  RDFResource(RDFResource &&other) noexcept
      : value(std::move(other.value)), resource_type(other.resource_type) {}

  RDFResource(const proto_msg::RDFTerm &term)
      : value(term.term_value()),
        resource_type(select_type_from_proto(term.term_type())) {}

  RDFResource &operator=(const RDFResource &other) {
    value = other.value;
    resource_type = other.resource_type;
    return *this;
  }

  RDFResource &operator=(RDFResource &&other) noexcept {
    value = std::move(other.value);
    resource_type = other.resource_type;
    return *this;
  }

  bool operator==(const RDFResource &other) const {
    return value == other.value && resource_type == other.resource_type;
  }

  static RDFResource null_resource() {
    return RDFResource("", NULL_RESOURCE_TYPE);
  }

  static RDFResourceType
  select_type_from_proto(proto_msg::TermType proto_type) {
    switch (proto_type) {
    case proto_msg::TermType::BLANK_NODE:
      return RDFResourceType::RDF_TYPE_BLANK;
    case proto_msg::TermType::IRI:
      return RDFResourceType::RDF_TYPE_IRI;
    case proto_msg::TermType::LITERAL:
      return RDFResourceType::RDF_TYPE_LITERAL;
    default:
      throw std::runtime_error("Unknown proto type: " +
                               std::to_string(proto_type));
    }
  }
  bool is_null() const { return resource_type == NULL_RESOURCE_TYPE; }
  proto_msg::TermType get_proto_type() {
    switch (resource_type) {
    case RDF_TYPE_IRI:
      return proto_msg::TermType::IRI;
    case RDF_TYPE_BLANK:
      return proto_msg::TermType::BLANK_NODE;
    case RDF_TYPE_LITERAL:
      return proto_msg::TermType::LITERAL;
    case NULL_RESOURCE_TYPE:
      return proto_msg::TermType::UNKNOWN_TERM_TYPE;
    }
    return proto_msg::TermType::UNKNOWN_TERM_TYPE;
  }
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
      : subject(subject), predicate(predicate), object(object) {}
  explicit RDFTripleResource(const proto_msg::TripleNode &triple_node)
      : subject(triple_node.subject()), predicate(triple_node.predicate()),
        object(triple_node.object()) {}
};

#endif // RDFCACHEK2_RDFTRIPLE_HPP
