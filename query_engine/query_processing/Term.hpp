//
// Created by Cristobal Miranda, 2020
//

#ifndef RDFCACHEK2_TERM_HPP
#define RDFCACHEK2_TERM_HPP

#include <request_msg.pb.h>

enum TermType {
  IRI = proto_msg::TermType::IRI,
  BLANK = proto_msg::TermType::BLANK_NODE,
  LITERAL = proto_msg::TermType::LITERAL,
  VAR = proto_msg::VARIABLE
};

struct Term {
  TermType type;
  std::string value;
  unsigned long id_value;
};

#endif
