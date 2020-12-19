//
// Created by Cristobal Miranda, 2020
//
#include <iostream>
#include <random>
#include <memory>

#include <gtest/gtest.h>

#include <sparql_tree.pb.h>

#include <EmptyISDManager.hpp>
#include <PredicatesCacheManager.hpp>
#include <RDFTriple.hpp>
#include <Cache.hpp>

TEST(QueryProcTests, test1) {
  proto_msg::SparqlTree tree;
  auto *project_node = tree.mutable_root()->mutable_project_node();
  project_node->add_vars("?x");
  auto *bgp = project_node->mutable_sub_op()->mutable_bgp_node();
  //proto_msg::TripleNode().mutable_subject()
  auto *first_triple = bgp->mutable_triple()->Add();
  auto *first_subject = first_triple->mutable_subject();
  first_subject->set_term_type(proto_msg::TermType::VARIABLE);
  first_subject->set_basic_type(proto_msg::BasicType::STRING);
  first_subject->set_term_value("?x");
  
  auto *first_predicate = first_triple->mutable_predicate();
  first_predicate->set_term_type(proto_msg::TermType::IRI);
  first_predicate->set_basic_type(proto_msg::BasicType::STRING);
  first_predicate->set_term_value("<pred1>");
  
  auto *first_object = first_triple->mutable_object();
  first_object->set_term_type(proto_msg::TermType::LITERAL);
  first_object->set_basic_type(proto_msg::BasicType::STRING);
  first_object->set_term_value("obj1");

  auto string_result =  project_node->DebugString();
  std::cout << string_result << std::endl;


  auto pcm = std::make_unique<PredicatesCacheManager>(std::make_unique<EmptyISDManager>());
  pcm->add_triple(RDFTripleResource(RDFResource("<subj1>", RDF_TYPE_IRI),
                                   RDFResource("<pred1>", RDF_TYPE_IRI),
                                   RDFResource("obj1", RDF_TYPE_LITERAL)));

  

  Cache cache(std::move(pcm));

  std::cout << "running query..." << std::endl;

  auto result = cache.run_query(tree);

  auto &table = result.table();
  for(auto &row : table.data)
  {
    
  }
}
