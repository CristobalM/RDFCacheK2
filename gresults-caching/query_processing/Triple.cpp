#include "Triple.hpp"

static RDFResourceType resource_type_from_proto_type(proto_msg::TermType type) {
  switch (type) {
  case proto_msg::TermType::IRI:
    return RDFResourceType::RDF_TYPE_IRI;
  case proto_msg::TermType::LITERAL:
    return RDFResourceType::RDF_TYPE_LITERAL;
  case proto_msg::TermType::BLANK_NODE:
    return RDFResourceType::RDF_TYPE_BLANK;
  default:
    throw std::runtime_error("Unknown TermType " + std::to_string(type));
  }
}

Triple::Triple(const proto_msg::TripleNode &proto_triple,
               const PredicatesCacheManager &cm) {
  subject.type = static_cast<TermType>(proto_triple.subject().term_type());
  subject.value = proto_triple.subject().term_value();
  subject.id_value =
      subject.type != TermType::VAR
          ? cm.get_resource_index(RDFResource(
                subject.value, resource_type_from_proto_type(
                                   proto_triple.subject().term_type())))
          : 0;

  predicate.type = static_cast<TermType>(proto_triple.predicate().term_type());
  predicate.value = proto_triple.predicate().term_value();
  predicate.id_value =
      predicate.type != TermType::VAR
          ? cm.get_resource_index(RDFResource(
                predicate.value, resource_type_from_proto_type(
                                     proto_triple.predicate().term_type())))
          : 0;

  object.type = static_cast<TermType>(proto_triple.object().term_type());
  object.value = proto_triple.object().term_value();
  object.id_value =
      object.type != TermType::VAR
          ? cm.get_resource_index(RDFResource(
                object.value, resource_type_from_proto_type(
                                  proto_triple.object().term_type())))
          : 0;
}
