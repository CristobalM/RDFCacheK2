//
// Created by Cristobal Miranda, 2020
//

#ifndef RDFCACHEK2_QUERYPROC_TRIPLE_HPP
#define RDFCACHEK2_QUERYPROC_TRIPLE_HPP

#include "PredicatesCacheManager.hpp"
#include "Term.hpp"
#include <request_msg.pb.h>

struct Triple {
  Term subject, predicate, object;
  Triple(const proto_msg::TripleNode &proto_triple,
         const PredicatesCacheManager &cm);
};

#endif
