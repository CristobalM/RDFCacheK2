//
// Created by Cristobal Miranda, 2020
//
#include <iostream>
#include <memory>
#include <random>

#include <gtest/gtest.h>

#include <sparql_tree.pb.h>

#include <Cache.hpp>
#include <EmptyISDManager.hpp>
#include <PredicatesCacheManager.hpp>
#include <RDFTriple.hpp>

TEST(QueryProcTests, test_bgp_node_1) {
  proto_msg::SparqlTree tree;
  auto *project_node = tree.mutable_root()->mutable_project_node();
  project_node->add_vars("?x");
  auto *bgp = project_node->mutable_sub_op()->mutable_bgp_node();
  // proto_msg::TripleNode().mutable_subject()
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

  auto string_result = project_node->DebugString();
  std::cout << string_result << std::endl;

  auto pcm = std::make_unique<PredicatesCacheManager>(
      std::make_unique<EmptyISDManager>());
  pcm->add_triple(RDFTripleResource(RDFResource("<subj1>", RDF_TYPE_IRI),
                                    RDFResource("<pred1>", RDF_TYPE_IRI),
                                    RDFResource("obj1", RDF_TYPE_LITERAL)));
  pcm->add_triple(RDFTripleResource(RDFResource("<subj3>", RDF_TYPE_IRI),
                                    RDFResource("<pred1>", RDF_TYPE_IRI),
                                    RDFResource("obj1", RDF_TYPE_LITERAL)));
  pcm->add_triple(RDFTripleResource(RDFResource("<subj2>", RDF_TYPE_IRI),
                                    RDFResource("<pred1>", RDF_TYPE_IRI),
                                    RDFResource("obj1", RDF_TYPE_LITERAL)));

  Cache cache(std::move(pcm));

  std::cout << "running query..." << std::endl;

  auto result = cache.run_query(tree);

  auto &vim = result.get_vim();
  auto reverse_map = vim.reverse();
  auto &table = result.table();
  for (auto col : table.headers) {
    std::cout << reverse_map[col] << " ";
  }
  std::cout << "\n";
  for (auto &row : table.data) {
    for (auto col : row) {
      auto col_resource = cache.extract_resource(col);
      std::cout << col_resource << " ";
    }
    std::cout << "\n";
  }
  std::cout << std::endl;
}

TEST(QueryProcTests, test_bgp_node_2) {
  proto_msg::SparqlTree tree;
  auto *project_node = tree.mutable_root()->mutable_project_node();
  project_node->add_vars("?x");
  project_node->add_vars("?y");
  auto *bgp = project_node->mutable_sub_op()->mutable_bgp_node();
  // proto_msg::TripleNode().mutable_subject()
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

  auto *second_triple = bgp->mutable_triple()->Add();
  auto *second_subject = second_triple->mutable_subject();
  second_subject->set_term_type(proto_msg::TermType::VARIABLE);
  second_subject->set_basic_type(proto_msg::BasicType::STRING);
  second_subject->set_term_value("?x");

  auto *second_predicate = second_triple->mutable_predicate();
  second_predicate->set_term_type(proto_msg::TermType::IRI);
  second_predicate->set_basic_type(proto_msg::BasicType::STRING);
  second_predicate->set_term_value("<pred2>");

  auto *second_object = second_triple->mutable_object();
  second_object->set_term_type(proto_msg::TermType::VARIABLE);
  second_object->set_basic_type(proto_msg::BasicType::STRING);
  second_object->set_term_value("?y");


  auto string_result = project_node->DebugString();
  std::cout << string_result << std::endl;

  auto pcm = std::make_unique<PredicatesCacheManager>(
      std::make_unique<EmptyISDManager>());
  pcm->add_triple(RDFTripleResource(RDFResource("<subj1>", RDF_TYPE_IRI),
                                    RDFResource("<pred1>", RDF_TYPE_IRI),
                                    RDFResource("obj1", RDF_TYPE_LITERAL)));

  pcm->add_triple(RDFTripleResource(RDFResource("<subj2>", RDF_TYPE_IRI),
                                    RDFResource("<pred1>", RDF_TYPE_IRI),
                                    RDFResource("obj1", RDF_TYPE_LITERAL)));
  pcm->add_triple(RDFTripleResource(RDFResource("<subj2>", RDF_TYPE_IRI),
                                    RDFResource("<pred1>", RDF_TYPE_IRI),
                                    RDFResource("obj2", RDF_TYPE_LITERAL)));

  pcm->add_triple(RDFTripleResource(RDFResource("<subj3>", RDF_TYPE_IRI),
                                    RDFResource("<pred1>", RDF_TYPE_IRI),
                                    RDFResource("obj3", RDF_TYPE_LITERAL)));

  pcm->add_triple(RDFTripleResource(RDFResource("<subj2>", RDF_TYPE_IRI),
                                    RDFResource("<pred2>", RDF_TYPE_IRI),
                                    RDFResource("obj1", RDF_TYPE_LITERAL)));
  pcm->add_triple(RDFTripleResource(RDFResource("<subj2>", RDF_TYPE_IRI),
                                    RDFResource("<pred2>", RDF_TYPE_IRI),
                                    RDFResource("obj2", RDF_TYPE_LITERAL)));

  pcm->add_triple(RDFTripleResource(RDFResource("<subj3>", RDF_TYPE_IRI),
                                    RDFResource("<pred2>", RDF_TYPE_IRI),
                                    RDFResource("obj2", RDF_TYPE_LITERAL)));

  Cache cache(std::move(pcm));

  std::cout << "running query..." << std::endl;

  auto result = cache.run_query(tree);

  auto &vim = result.get_vim();
  auto reverse_map = vim.reverse();
  auto &table = result.table();
  for (auto col : table.headers) {
    std::cout << reverse_map[col] << " ";
  }
  std::cout << "\n";
  for (auto &row : table.data) {
    for (auto col : row) {
      auto col_resource = cache.extract_resource(col);
      std::cout << col_resource << " ";
    }
    std::cout << "\n";
  }
  std::cout << std::endl;
}
