//
// Created by Cristobal Miranda, 2020
//

#include <gtest/gtest.h>
#include <memory>

#include <EmptyISDManager.hpp>
#include <PredicatesCacheManager.hpp>
#include <RDFTriple.hpp>

TEST(predicates_cache_manager_test, test1) {
  K2TreeConfig config;
  config.treedepth = 32;
  config.max_node_count = 256;
  config.cut_depth = 10;
  std::string fname = "predicates.bin";
  PredicatesCacheManager pcm(std::make_unique<EmptyISDManager>(), config, fname);
  pcm.add_triple(RDFTripleResource(RDFResource("subject1", RDF_TYPE_IRI),
                                   RDFResource("predicate1", RDF_TYPE_IRI),
                                   RDFResource("object1", RDF_TYPE_LITERAL)));

}
