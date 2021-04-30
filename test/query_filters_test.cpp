//
// Created by cristobal on 4/30/21.
//

#include "cache_test_util.hpp"
#include <Cache.hpp>
#include <EmptyISDManager.hpp>
#include <PredicatesCacheManager.hpp>
#include <filesystem>
#include <gtest/gtest.h>
#include <memory>

namespace fs = std::filesystem;
class QueryFiltersFixture : public ::testing::Test {
protected:
  virtual void SetUp() override {}
  virtual void TearDown() override {}
  virtual void TestBody() override {}

public:
  static std::string fname;
  static std::shared_ptr<PredicatesCacheManager> pcm;
  static std::unique_ptr<Cache> cache;
  static void SetUpTestCase() {
    fname = "predicates.bin";
    build_cache_test_file(fname);
    pcm = std::make_shared<PredicatesCacheManager>(
        std::make_unique<EmptyISDManager>(), fname);
    pcm->add_triple(RDFTripleResource(
        RDFResource("<some_integer_ref1>", RDFResourceType::RDF_TYPE_IRI),
        RDFResource("<has_integer>", RDFResourceType::RDF_TYPE_IRI),
        RDFResource("\"42\"^^xsd:integer", RDFResourceType::RDF_TYPE_LITERAL)));
    pcm->add_triple(RDFTripleResource(
        RDFResource("<some_integer_ref1>", RDFResourceType::RDF_TYPE_IRI),
        RDFResource("<has_integer>", RDFResourceType::RDF_TYPE_IRI),
        RDFResource("\"1\"^^xsd:integer", RDFResourceType::RDF_TYPE_LITERAL)));
    pcm->add_triple(RDFTripleResource(
        RDFResource("<some_integer_ref1>", RDFResourceType::RDF_TYPE_IRI),
        RDFResource("<has_integer>", RDFResourceType::RDF_TYPE_IRI),
        RDFResource("\"2\"^^xsd:integer", RDFResourceType::RDF_TYPE_LITERAL)));
    pcm->add_triple(RDFTripleResource(
        RDFResource("<some_integer_ref1>", RDFResourceType::RDF_TYPE_IRI),
        RDFResource("<has_integer>", RDFResourceType::RDF_TYPE_IRI),
        RDFResource("\"3\"^^xsd:integer", RDFResourceType::RDF_TYPE_LITERAL)));
    pcm->add_triple(RDFTripleResource(
        RDFResource("<some_integer_ref1>", RDFResourceType::RDF_TYPE_IRI),
        RDFResource("<has_integer>", RDFResourceType::RDF_TYPE_IRI),
        RDFResource("\"-1\"^^xsd:integer", RDFResourceType::RDF_TYPE_LITERAL)));
    pcm->add_triple(RDFTripleResource(
        RDFResource("<some_integer_ref1>", RDFResourceType::RDF_TYPE_IRI),
        RDFResource("<has_integer>", RDFResourceType::RDF_TYPE_IRI),
        RDFResource("\"4\"^^xsd:integer", RDFResourceType::RDF_TYPE_LITERAL)));
    pcm->add_triple(RDFTripleResource(
        RDFResource("<some_integer_ref1>", RDFResourceType::RDF_TYPE_IRI),
        RDFResource("<has_integer>", RDFResourceType::RDF_TYPE_IRI),
        RDFResource("\"400\"^^xsd:integer",
                    RDFResourceType::RDF_TYPE_LITERAL)));
    pcm->add_triple(RDFTripleResource(
        RDFResource("<some_integer_ref1>", RDFResourceType::RDF_TYPE_IRI),
        RDFResource("<has_integer>", RDFResourceType::RDF_TYPE_IRI),
        RDFResource("\"98\"^^xsd:integer", RDFResourceType::RDF_TYPE_LITERAL)));

    pcm->add_triple(RDFTripleResource(
        RDFResource("<some_integer_ref1>", RDFResourceType::RDF_TYPE_IRI),
        RDFResource("<has_integer>", RDFResourceType::RDF_TYPE_IRI),
        RDFResource("\"99\"^^xsd:integer", RDFResourceType::RDF_TYPE_LITERAL)));

    cache =
        std::make_unique<Cache>(pcm, CacheReplacement::STRATEGY::LRU, 100'000);
  }

  static void TearDownTestCase() {
    fs::remove(fname);
    pcm = nullptr;
    cache = nullptr;
  }
};

std::string QueryFiltersFixture::fname = std::string();
std::shared_ptr<PredicatesCacheManager> QueryFiltersFixture::pcm = nullptr;
std::unique_ptr<Cache> QueryFiltersFixture::cache = nullptr;

TEST_F(QueryFiltersFixture, test_add_filter_1) {
  proto_msg::SparqlTree tree;
  auto *project_node = tree.mutable_root()->mutable_project_node();
  project_node->add_vars("?x");
  auto *filter_node = project_node->mutable_sub_op()->mutable_filter_node();
  auto *bgp_node = filter_node->mutable_node()->mutable_bgp_node();
  auto *first_triple = bgp_node->mutable_triple()->Add();
  auto *first_subject = first_triple->mutable_subject();
  auto *first_predicate = first_triple->mutable_predicate();
  auto *first_object = first_triple->mutable_object();
  first_subject->set_term_value("<some_integer_ref1>");
  first_subject->set_term_type(proto_msg::TermType::IRI);
  first_predicate->set_term_value("<has_integer>");
  first_predicate->set_term_type(proto_msg::TermType::IRI);
  first_object->set_term_value("?x");
  first_object->set_term_type(proto_msg::TermType::VARIABLE);

  auto *less_than_expr = filter_node->mutable_exprs()->Add();
  auto *less_than_expr_fnode = less_than_expr->mutable_function_node();
  less_than_expr_fnode->set_function_op(proto_msg::FunctionOP::LESS_THAN);
  auto *add_expr = less_than_expr_fnode->mutable_exprs()->Add();
  auto *add_expr_fnode = add_expr->mutable_function_node();
  add_expr_fnode->set_function_op(proto_msg::FunctionOP::ADD);
  auto *add_expr_first = add_expr_fnode->mutable_exprs()->Add();
  auto *add_expr_second = add_expr_fnode->mutable_exprs()->Add();

  auto *add_expr_first_var = add_expr_first->mutable_term_node();
  add_expr_first_var->set_term_value("?x");
  add_expr_first_var->set_term_type(proto_msg::TermType::VARIABLE);
  auto *add_expr_second_val = add_expr_second->mutable_term_node();
  add_expr_second_val->set_term_value("\"1\"^^xsd:integer");
  add_expr_second_val->set_term_type(proto_msg::TermType::LITERAL);

  auto *second_expr_lt = less_than_expr_fnode->mutable_exprs()->Add();
  auto *second_expr_lt_term = second_expr_lt->mutable_term_node();
  second_expr_lt_term->set_term_value("\"100\"^^xsd:integer");
  second_expr_lt_term->set_term_type(proto_msg::TermType::LITERAL);

  std::cout << tree.DebugString() << std::endl;

  auto result = cache->run_query(tree);

  print_table_debug2(result, *cache);
}