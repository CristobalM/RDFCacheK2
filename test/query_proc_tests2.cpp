//
// Created by cristobal on 5/17/21.
//

#include <cmath>
#include <filesystem>
#include <gtest/gtest.h>
#include <memory>

#include "cache_test_util.hpp"
#include <curl/curl.h>

#include <Cache.hpp>
#include <EmptyISDManager.hpp>
#include <PredicatesCacheManager.hpp>
#include <hashing.hpp>
#include <query_processing/expr/StringHandlingUtil.hpp>

namespace fs = std::filesystem;
class QueryProcTests2Fixture : public ::testing::Test {
protected:
  virtual void SetUp() override {}
  virtual void TearDown() override {}
  virtual void TestBody() override {}

public:
  static std::string fname;
  static std::shared_ptr<PredicatesCacheManager> pcm;
  static std::unique_ptr<Cache> cache;
  static std::set<int> values1;
  static std::set<int> values2;
  static std::set<double> values_double1;
  static std::vector<std::string> dates;
  static std::vector<std::string> some_literal_strings1;
  static std::vector<DateInfo> date_infos_expected;

  static void SetUpTestCase() {
    fname = "predicates.bin";
    build_cache_test_file(fname);
    pcm = std::make_shared<PredicatesCacheManager>(
        std::make_unique<EmptyISDManager>(), fname);
    values1 = {-100, 42, 1, 2, 3, -1, 4, 400, 98, 99, 49, 50, -30};
    values2 = {400, 98, 99, 49, 50, -30};
    values_double1 = {-100.54, 42.33,    1.123,   2.65,     3.14,    -1.024,
                      4.99,    400.8974, 98.0023, 99.00005, 49.0123, 50.99991};
    for (auto value : values1) {
      pcm->add_triple(RDFTripleResource(
          RDFResource("<some_integer_ref1>", RDFResourceType::RDF_TYPE_IRI),
          RDFResource("<has_integer>", RDFResourceType::RDF_TYPE_IRI),
          RDFResource("\"" + std::to_string(value) + "\"^^xsd:integer",
                      RDFResourceType::RDF_TYPE_LITERAL)));
    }

    for (auto value : values2) {
      pcm->add_triple(RDFTripleResource(
          RDFResource("<some_integer_ref2>", RDFResourceType::RDF_TYPE_IRI),
          RDFResource("<has_integer>", RDFResourceType::RDF_TYPE_IRI),
          RDFResource("\"" + std::to_string(value) + "\"^^xsd:integer",
                      RDFResourceType::RDF_TYPE_LITERAL)));
    }
    for (auto value : values_double1) {
      pcm->add_triple(RDFTripleResource(
          RDFResource("<some_double_ref1>", RDFResourceType::RDF_TYPE_IRI),
          RDFResource("<has_double>", RDFResourceType::RDF_TYPE_IRI),
          RDFResource("\"" + std::to_string(value) + "\"^^xsd:double",
                      RDFResourceType::RDF_TYPE_LITERAL)));
    }

    dates = {
        "2021-05-04T19:50:54.988Z",      "2022-01-02T11:20:34.988Z",
        "2023-05-05T19:50:44.988Z",      "2024-05-09T19:50:44.988Z",
        "2025-05-04T19:50:44.988Z",      "2026-12-02T19:50:44.988Z",
        "2025-05-04T19:50:44.988-05:31", "2026-12-02T19:50:44.988+09:31",
    };

    some_literal_strings1 = {
        "first_string",          "second_string",         "third_string",
        "fourth_string",         "fifth_string",          "sixth_string",
        "seventh_string",        "eighth_string",         "tenth_string",
        "SOME_UPPERCASE_STRING", "some_lowercase_string",
    };
    for (const auto &date : dates) {
      date_infos_expected.push_back(
          ExprProcessorPersistentData::get().parse_iso8601(date));
    }

    for (const auto &date : dates) {
      pcm->add_triple(RDFTripleResource(
          RDFResource("<some_date_ref1>", RDFResourceType::RDF_TYPE_IRI),
          RDFResource("<has_date>", RDFResourceType::RDF_TYPE_IRI),
          RDFResource("\"" + date + "\"^^xsd:dateTime",
                      RDFResourceType::RDF_TYPE_LITERAL)));
    }

    pcm->add_triple(RDFTripleResource(
        RDFResource("<some_iri_referencing_a_blank>",
                    RDFResourceType::RDF_TYPE_IRI),
        RDFResource("<has_blank>", RDFResourceType::RDF_TYPE_IRI),
        RDFResource("_:someBlankStuff", RDFResourceType::RDF_TYPE_BLANK)));

    pcm->add_triple(RDFTripleResource(
        RDFResource("<some_iri_referencing_a_literal",
                    RDFResourceType::RDF_TYPE_IRI),
        RDFResource("<has_literal>", RDFResourceType::RDF_TYPE_IRI),
        RDFResource("\"someLiteralStuff\"",
                    RDFResourceType::RDF_TYPE_LITERAL)));

    for (const auto &str : some_literal_strings1) {
      pcm->add_triple(RDFTripleResource(
          RDFResource("<some_iri_referencing_a_str_literal>",
                      RDFResourceType::RDF_TYPE_IRI),
          RDFResource("<has_str_literal>", RDFResourceType::RDF_TYPE_IRI),
          RDFResource("\"" + str + "\"", RDFResourceType::RDF_TYPE_LITERAL)));
    }

    cache =
        std::make_unique<Cache>(pcm, CacheReplacement::STRATEGY::LRU, 100'000);
  }

  static void TearDownTestCase() {
    fs::remove(fname);
    pcm = nullptr;
    cache = nullptr;
  }
};

std::string QueryProcTests2Fixture::fname;
std::shared_ptr<PredicatesCacheManager> QueryProcTests2Fixture::pcm;
std::unique_ptr<Cache> QueryProcTests2Fixture::cache;
std::set<int> QueryProcTests2Fixture::values1;
std::set<int> QueryProcTests2Fixture::values2;
std::set<double> QueryProcTests2Fixture::values_double1;
std::vector<std::string> QueryProcTests2Fixture::dates;
std::vector<std::string> QueryProcTests2Fixture::some_literal_strings1;
std::vector<DateInfo> QueryProcTests2Fixture::date_infos_expected;

/*
static std::set<std::string>
get_string_values_from_result_table(const ResultTable &result_table,
                                    const PredicatesCacheManager &pcm) {
  std::set<std::string> results;
  if (result_table.headers.size() != 1)
    throw std::runtime_error("result_table must have 1 column");
  for (const auto &row : result_table.data) {
    auto resource = pcm.extract_resource(row[0]);
    results.insert(resource.value);
  }
  return results;
}
 */

static std::set<int>
get_values_from_result_table(const ResultTable &result_table,
                             const PredicatesCacheManager &pcm) {
  std::set<int> result;
  if (result_table.headers.size() != 1)
    throw std::runtime_error("result_table must have 1 column");
  for (const auto &row : result_table.data) {
    auto resource = pcm.extract_resource(row[0]);
    auto literal_data =
        StringHandlingUtil::extract_literal_data_from_rdf_resource(resource);
    if (literal_data.type != EDT_INTEGER)
      throw std::runtime_error("Expected integer resource");
    result.insert(std::stoi(literal_data.value));
  }
  return result;
}

TEST_F(QueryProcTests2Fixture, can_do_minus_op_test_1) {
  proto_msg::SparqlTree tree;
  auto *distinct_node = tree.mutable_root()->mutable_distinct_node();
  auto *project_node =
      distinct_node->mutable_sub_node()->mutable_project_node();
  project_node->add_vars("?x");
  auto *minus_node = project_node->mutable_sub_op()->mutable_minus_node();
  auto *bgp_node_left = minus_node->mutable_left_node()->mutable_bgp_node();
  auto *left_first_triple = bgp_node_left->mutable_triple()->Add();
  auto *left_first_subject = left_first_triple->mutable_subject();
  auto *left_first_object = left_first_triple->mutable_object();
  auto *left_first_predicate = left_first_triple->mutable_predicate();

  left_first_subject->set_term_type(proto_msg::TermType::VARIABLE);
  left_first_subject->set_term_value("?y");
  left_first_predicate->set_term_value("<has_integer>");
  left_first_predicate->set_term_type(proto_msg::TermType::IRI);
  left_first_object->set_term_value("?x");
  left_first_object->set_term_type(proto_msg::TermType::VARIABLE);

  auto *filter_node = minus_node->mutable_right_node()->mutable_filter_node();
  auto *bgp_node = filter_node->mutable_node()->mutable_bgp_node();
  auto *first_triple = bgp_node->mutable_triple()->Add();
  auto *first_subject = first_triple->mutable_subject();
  auto *first_predicate = first_triple->mutable_predicate();
  auto *first_object = first_triple->mutable_object();
  first_subject->set_term_value("?y");
  first_subject->set_term_type(proto_msg::TermType::VARIABLE);
  first_predicate->set_term_value("<has_integer>");
  first_predicate->set_term_type(proto_msg::TermType::IRI);
  first_object->set_term_value("?x");
  first_object->set_term_type(proto_msg::TermType::VARIABLE);

  auto *equals_fnode =
      filter_node->mutable_exprs()->Add()->mutable_function_node();
  equals_fnode->set_function_op(proto_msg::FunctionOP::LESS_THAN);

  auto *lhs_equality_term =
      equals_fnode->mutable_exprs()->Add()->mutable_term_node();
  auto *rhs_equality_term =
      equals_fnode->mutable_exprs()->Add()->mutable_term_node();

  lhs_equality_term->set_term_value("?x");
  lhs_equality_term->set_term_type(proto_msg::TermType::VARIABLE);

  rhs_equality_term->set_term_value("\"0\"^^xsd:integer");
  rhs_equality_term->set_term_type(proto_msg::TermType::LITERAL);

  auto result = QueryProcTests2Fixture::cache->run_query(tree);

  auto query_values_set = get_values_from_result_table(
      result.table(), *QueryProcTests2Fixture::pcm);

  std::set<int> expected_values;
  for (auto value : values1) {
    if (value > 0) {
      expected_values.insert(value);
    }
  }
  ASSERT_GT(query_values_set.size(), 0);
  ASSERT_EQ(query_values_set, expected_values);
}

TEST_F(QueryProcTests2Fixture, can_do_sequence_op_test_1) {
  proto_msg::SparqlTree tree;
  auto *distinct_node = tree.mutable_root()->mutable_distinct_node();
  auto *project_node =
      distinct_node->mutable_sub_node()->mutable_project_node();
  project_node->add_vars("?x");
  project_node->add_vars("?w");
  auto *sequence_node = project_node->mutable_sub_op()->mutable_sequence_node();
  auto *bgp_node_left =
      sequence_node->mutable_nodes()->Add()->mutable_bgp_node();
  auto *left_first_triple = bgp_node_left->mutable_triple()->Add();
  auto *left_first_subject = left_first_triple->mutable_subject();
  auto *left_first_object = left_first_triple->mutable_object();
  auto *left_first_predicate = left_first_triple->mutable_predicate();

  left_first_subject->set_term_type(proto_msg::TermType::IRI);
  left_first_subject->set_term_value("<some_integer_ref1>");
  left_first_predicate->set_term_value("<has_integer>");
  left_first_predicate->set_term_type(proto_msg::TermType::IRI);
  left_first_object->set_term_value("?x");
  left_first_object->set_term_type(proto_msg::TermType::VARIABLE);

  auto *bgp_node = sequence_node->mutable_nodes()->Add()->mutable_bgp_node();
  auto *first_triple = bgp_node->mutable_triple()->Add();
  auto *second_triple = bgp_node->mutable_triple()->Add();
  auto *first_subject = first_triple->mutable_subject();
  auto *first_predicate = first_triple->mutable_predicate();
  auto *first_object = first_triple->mutable_object();
  auto *second_subject = second_triple->mutable_subject();
  auto *second_predicate = second_triple->mutable_predicate();
  auto *second_object = second_triple->mutable_object();

  first_subject->set_term_value("<some_integer_ref2>");
  first_subject->set_term_type(proto_msg::TermType::IRI);
  first_predicate->set_term_value("<has_integer>");
  first_predicate->set_term_type(proto_msg::TermType::IRI);
  first_object->set_term_value("?x");
  first_object->set_term_type(proto_msg::TermType::VARIABLE);

  second_subject->set_term_value("?uu");
  second_subject->set_term_type(proto_msg::TermType::VARIABLE);
  second_predicate->set_term_value("<has_date>");
  second_predicate->set_term_type(proto_msg::TermType::IRI);
  second_object->set_term_value("?w");
  second_object->set_term_type(proto_msg::TermType::VARIABLE);

  auto result = QueryProcTests2Fixture::cache->run_query(tree);

  std::set<std::pair<std::string, std::string>> expected_results;
  std::set<int> values1_2_intersection;
  std::set_intersection(
      values1.begin(), values1.end(), values2.begin(), values2.end(),
      std::inserter(values1_2_intersection, values1_2_intersection.begin()));
  // This is the assumption between values1 and values2 that must not be broken
  // for this test. If this was broken, this test will have to change
  ASSERT_EQ(values1_2_intersection, values2);

  for (auto int_value : values2) {
    for (const auto &date_str : dates) {
      expected_results.insert(
          {"\"" + std::to_string(int_value) + "\"^^xsd:integer",
           "\"" + date_str + "\"^^xsd:dateTime"});
    }
  }
  std::set<std::pair<std::string, std::string>> query_results_values;
  for (const auto &row : result.table().data) {
    auto first_resource = pcm->extract_resource(row[0]);
    auto second_resource = pcm->extract_resource(row[1]);
    query_results_values.insert(
        {std::move(first_resource.value), std::move(second_resource.value)});
  }

  ASSERT_EQ(query_results_values, expected_results);
}

TEST_F(QueryProcTests2Fixture, can_do_slice_op_test_1) {
  proto_msg::SparqlTree tree;
  auto *slice_node = tree.mutable_root()->mutable_slice_node();
  slice_node->set_start(5);
  slice_node->set_length(5);
  auto *distinct_node = slice_node->mutable_node()->mutable_distinct_node();
  auto *project_node =
      distinct_node->mutable_sub_node()->mutable_project_node();
  project_node->add_vars("?x");
  project_node->add_vars("?w");
  auto *sequence_node = project_node->mutable_sub_op()->mutable_sequence_node();
  auto *bgp_node_left =
      sequence_node->mutable_nodes()->Add()->mutable_bgp_node();
  auto *left_first_triple = bgp_node_left->mutable_triple()->Add();
  auto *left_first_subject = left_first_triple->mutable_subject();
  auto *left_first_object = left_first_triple->mutable_object();
  auto *left_first_predicate = left_first_triple->mutable_predicate();

  left_first_subject->set_term_type(proto_msg::TermType::IRI);
  left_first_subject->set_term_value("<some_integer_ref1>");
  left_first_predicate->set_term_value("<has_integer>");
  left_first_predicate->set_term_type(proto_msg::TermType::IRI);
  left_first_object->set_term_value("?x");
  left_first_object->set_term_type(proto_msg::TermType::VARIABLE);

  auto *bgp_node = sequence_node->mutable_nodes()->Add()->mutable_bgp_node();
  auto *first_triple = bgp_node->mutable_triple()->Add();
  auto *second_triple = bgp_node->mutable_triple()->Add();
  auto *first_subject = first_triple->mutable_subject();
  auto *first_predicate = first_triple->mutable_predicate();
  auto *first_object = first_triple->mutable_object();
  auto *second_subject = second_triple->mutable_subject();
  auto *second_predicate = second_triple->mutable_predicate();
  auto *second_object = second_triple->mutable_object();

  first_subject->set_term_value("<some_integer_ref2>");
  first_subject->set_term_type(proto_msg::TermType::IRI);
  first_predicate->set_term_value("<has_integer>");
  first_predicate->set_term_type(proto_msg::TermType::IRI);
  first_object->set_term_value("?x");
  first_object->set_term_type(proto_msg::TermType::VARIABLE);

  second_subject->set_term_value("?uu");
  second_subject->set_term_type(proto_msg::TermType::VARIABLE);
  second_predicate->set_term_value("<has_date>");
  second_predicate->set_term_type(proto_msg::TermType::IRI);
  second_object->set_term_value("?w");
  second_object->set_term_type(proto_msg::TermType::VARIABLE);

  auto result = QueryProcTests2Fixture::cache->run_query(tree);

  std::set<std::pair<std::string, std::string>> all_combinations;
  std::set<int> values1_2_intersection;
  std::set_intersection(
      values1.begin(), values1.end(), values2.begin(), values2.end(),
      std::inserter(values1_2_intersection, values1_2_intersection.begin()));
  // This is the assumption between values1 and values2 that must not be broken
  // for this test. If this was broken, this test will have to change
  ASSERT_EQ(values1_2_intersection, values2);

  for (auto int_value : values2) {
    for (const auto &date_str : dates) {
      all_combinations.insert(
          {"\"" + std::to_string(int_value) + "\"^^xsd:integer",
           "\"" + date_str + "\"^^xsd:dateTime"});
    }
  }
  std::set<std::pair<std::string, std::string>> query_results_values;
  for (const auto &row : result.table().data) {
    auto first_resource = pcm->extract_resource(row[0]);
    auto second_resource = pcm->extract_resource(row[1]);
    query_results_values.insert(
        {std::move(first_resource.value), std::move(second_resource.value)});
  }

  ASSERT_TRUE(std::includes(all_combinations.begin(), all_combinations.end(),
                            query_results_values.begin(),
                            query_results_values.end()));
  ASSERT_EQ(query_results_values.size(), 5);
}
