//
// Created by Cristobal Miranda, 2020
//

#include <gtest/gtest.h>
#include <memory>
#include <filesystem>

#include <EmptyISDManager.hpp>
#include <PredicatesCacheManager.hpp>
#include <RDFTriple.hpp>
#include <external_sort.hpp>
#include <serialization_util.hpp>
#include <PredicatesIndexFileBuilder.hpp>
#include <K2TreeMixed.hpp>
#include "cache_test_util.hpp"

TEST(predicates_cache_manager_test, test1) {
  K2TreeConfig config;
  config.treedepth = 32;
  config.max_node_count = 256;
  config.cut_depth = 10;
  
  std::string fname = "predicates.bin";

  std::vector<TripleValue> data;
  uint64_t sz =  10000;
  for(uint64_t i = 1; i <= sz; i++){
    data.push_back(TripleValue(i,i,i));
  }
  build_cache_test_file(fname, data);
  PredicatesCacheManager pcm(std::make_unique<EmptyISDManager>(), config, fname);

  for(uint64_t i = 1; i <= sz; i++){
    // ASSERT_TRUE(pcm.has_triple(i, i, i));
    ASSERT_TRUE(pcm.get_predicates_index_cache().fetch_k2tree(i).has(i, i));
  }

  auto subject = RDFResource("subject1", RDF_TYPE_IRI);
  auto predicate = RDFResource("predicate1", RDF_TYPE_IRI);
  auto object = RDFResource("object1", RDF_TYPE_LITERAL);


  auto resource =  RDFTripleResource(subject,
                                   predicate,
                                   object);
  pcm.add_triple(resource);

  auto subject_index = pcm.get_resource_index(subject);
  auto predicate_index = pcm.get_resource_index(predicate);
  auto object_index = pcm.get_resource_index(object);

  ASSERT_TRUE(pcm.has_triple(resource));
  ASSERT_TRUE(pcm.get_predicates_index_cache().fetch_k2tree(predicate_index).has(subject_index, object_index));

  std::filesystem::remove(fname);
}
