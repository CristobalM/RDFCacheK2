//
// Created by Cristobal Miranda, 2020
//

#include <gtest/gtest.h>
#include <memory>

#include <EmptyISDManager.hpp>
#include <PredicatesCacheManager.hpp>
#include <RDFTriple.hpp>

TEST(predicates_cache_manager_test, test1) {
  PredicatesCacheManager pcm(std::make_unique<EmptyISDManager>());
  pcm.add_triple(RDFTripleResource(RDFResource("subject1", RDF_TYPE_IRI),
                                   RDFResource("predicate1", RDF_TYPE_IRI),
                                   RDFResource("object1", RDF_TYPE_LITERAL)));
}