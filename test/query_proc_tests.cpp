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

static std::vector<std::vector<std::string>> translate_table(ResultTable &input_table, Cache &cache){
  std::vector<std::vector<std::string>> translated_table;
  for(auto &row: input_table.data){
    std::vector<std::string> translated_row;
    for(auto col: row){
      auto col_resource = cache.extract_resource(col);
      translated_row.push_back(col_resource);
    }
    translated_table.push_back(std::move(translated_row));
  }
  return translated_table;
}

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


  auto result = cache.run_query(tree);

  auto &vim = result.get_vim();
  auto reverse_map = vim.reverse();
  auto &table = result.table();


  auto translated_table = translate_table(table, cache);
  auto header_str = reverse_map[table.headers[0]];
  ASSERT_EQ(header_str, "?x");

  ASSERT_EQ(translated_table[0][0], "<subj1>");
  ASSERT_EQ(translated_table[1][0], "<subj3>");
  ASSERT_EQ(translated_table[2][0], "<subj2>");

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

  auto result = cache.run_query(tree);

  auto &vim = result.get_vim();
  auto reverse_map = vim.reverse();
  auto &table = result.table();
 


  ASSERT_EQ(reverse_map[table.headers[0]], "?x");
  ASSERT_EQ(reverse_map[table.headers[1]], "?y");

  ASSERT_EQ(table.data.size(), 2);
  auto translated_table = translate_table(table, cache);

  ASSERT_EQ(translated_table[0][0], "<subj2>");
  ASSERT_EQ(translated_table[0][1], "obj1");
  ASSERT_EQ(translated_table[1][0], "<subj2>");
  ASSERT_EQ(translated_table[1][1], "obj2");

}

TEST(QueryProcTests, test_bgp_node_3) {
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
  first_object->set_term_type(proto_msg::TermType::VARIABLE);
  first_object->set_basic_type(proto_msg::BasicType::STRING);
  first_object->set_term_value("?y");

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

  auto *third_triple = bgp->mutable_triple()->Add();
  auto *third_subject = third_triple->mutable_subject();
  third_subject->set_term_type(proto_msg::TermType::VARIABLE);
  third_subject->set_basic_type(proto_msg::BasicType::STRING);
  third_subject->set_term_value("?x");

  auto *third_predicate = third_triple->mutable_predicate();
  third_predicate->set_term_type(proto_msg::TermType::IRI);
  third_predicate->set_basic_type(proto_msg::BasicType::STRING);
  third_predicate->set_term_value("<pred3>");

  auto *third_object = third_triple->mutable_object();
  third_object->set_term_type(proto_msg::TermType::VARIABLE);
  third_object->set_basic_type(proto_msg::BasicType::STRING);
  third_object->set_term_value("?z");


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

  pcm->add_triple(RDFTripleResource(RDFResource("<subj3>", RDF_TYPE_IRI),
                                    RDFResource("<pred3>", RDF_TYPE_IRI),
                                    RDFResource("obj2", RDF_TYPE_LITERAL)));

  pcm->add_triple(RDFTripleResource(RDFResource("<subj2>", RDF_TYPE_IRI),
                                    RDFResource("<pred3>", RDF_TYPE_IRI),
                                    RDFResource("obj1", RDF_TYPE_LITERAL)));

  Cache cache(std::move(pcm));


  auto result = cache.run_query(tree);

  auto &vim = result.get_vim();
  auto reverse_map = vim.reverse();
  auto &table = result.table();

  auto translated_table = translate_table(table, cache);

  std::vector<std::string> translated_headers;

  for (auto col : table.headers) {
    translated_headers.push_back(reverse_map[col]);
  }

  ASSERT_EQ(translated_headers[0], "?x");
  ASSERT_EQ(translated_headers[1], "?y");

  ASSERT_EQ(translated_table[0][0], "<subj2>");
  ASSERT_EQ(translated_table[0][1], "obj1");

  ASSERT_EQ(translated_table[1][0], "<subj2>");
  ASSERT_EQ(translated_table[1][1], "obj2");
}



TEST(QueryProcTests, test_bgp_node_4_compact_dicts) {


}