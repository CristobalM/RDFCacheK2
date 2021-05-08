//
// Created by cristobal on 4/30/21.
//

#include <cmath>
#include <filesystem>
#include <gtest/gtest.h>
#include <memory>

#include "cache_test_util.hpp"

#include <Cache.hpp>
#include <EmptyISDManager.hpp>
#include <PredicatesCacheManager.hpp>
#include <hashing.hpp>
#include <query_processing/expr/StringHandlingUtil.hpp>

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
  static std::set<int> values1;
  static std::set<double> values_double1;
  static std::vector<std::string> dates;
  static std::vector<DateInfo> date_infos_expected;

  static void SetUpTestCase() {
    fname = "predicates.bin";
    build_cache_test_file(fname);
    pcm = std::make_shared<PredicatesCacheManager>(
        std::make_unique<EmptyISDManager>(), fname);
    values1 = {-100, 42, 1, 2, 3, -1, 4, 400, 98, 99, 49, 50};
    values_double1 = {-100.54, 42.33,    1.123,   2.65,     3.14,    -1.024,
                      4.99,    400.8974, 98.0023, 99.00005, 49.0123, 50.99991};
    for (auto value : values1) {
      pcm->add_triple(RDFTripleResource(
          RDFResource("<some_integer_ref1>", RDFResourceType::RDF_TYPE_IRI),
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

    cache =
        std::make_unique<Cache>(pcm, CacheReplacement::STRATEGY::LRU, 100'000);
  }

  static void TearDownTestCase() {
    fs::remove(fname);
    pcm = nullptr;
    cache = nullptr;
  }
};

std::string QueryFiltersFixture::fname;
std::shared_ptr<PredicatesCacheManager> QueryFiltersFixture::pcm;
std::unique_ptr<Cache> QueryFiltersFixture::cache;
std::set<int> QueryFiltersFixture::values1;
std::set<double> QueryFiltersFixture::values_double1;
std::vector<std::string> QueryFiltersFixture::dates;
std::vector<DateInfo> QueryFiltersFixture::date_infos_expected;

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
static std::set<double>
get_values_double_from_result_table(const ResultTable &result_table,
                                    const PredicatesCacheManager &pcm) {
  std::set<double> result;
  if (result_table.headers.size() != 1)
    throw std::runtime_error("result_table must have 1 column");
  for (const auto &row : result_table.data) {
    auto resource = pcm.extract_resource(row[0]);
    auto literal_data =
        StringHandlingUtil::extract_literal_data_from_rdf_resource(resource);
    if (literal_data.type != EDT_DOUBLE)
      throw std::runtime_error("Expected double resource");
    result.insert(std::stod(literal_data.value));
  }
  return result;
}

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

  auto result = cache->run_query(tree);

  auto query_values_set = get_values_from_result_table(result.table(), *pcm);

  ASSERT_GE(values1.size(), query_values_set.size());
  ASSERT_TRUE(std::includes(values1.begin(), values1.end(),
                            query_values_set.begin(), query_values_set.end()));

  std::set<int> values1_gte_than_100;
  for (auto value : values1)
    if (value > 100)
      values1_gte_than_100.insert(value);

  std::set<int> gte_than_100_in_result;
  std::set_intersection(
      query_values_set.begin(), query_values_set.end(),
      values1_gte_than_100.begin(), values1_gte_than_100.end(),
      std::inserter(gte_than_100_in_result, gte_than_100_in_result.begin()));
  ASSERT_TRUE(gte_than_100_in_result.empty());
}

TEST_F(QueryFiltersFixture, test_subtract_filter_1) {
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
  less_than_expr_fnode->set_function_op(
      proto_msg::FunctionOP::LESS_THAN_OR_EQUAL);
  auto *add_expr = less_than_expr_fnode->mutable_exprs()->Add();
  auto *add_expr_fnode = add_expr->mutable_function_node();
  add_expr_fnode->set_function_op(proto_msg::FunctionOP::SUBSTRACT);
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
  second_expr_lt_term->set_term_value("\"0\"^^xsd:integer");
  second_expr_lt_term->set_term_type(proto_msg::TermType::LITERAL);

  auto result = cache->run_query(tree);

  auto query_values_set = get_values_from_result_table(result.table(), *pcm);

  ASSERT_GE(values1.size(), query_values_set.size());
  ASSERT_TRUE(std::includes(values1.begin(), values1.end(),
                            query_values_set.begin(), query_values_set.end()));

  std::set<int> values1_gt_0;
  for (auto value : values1)
    if (value - 1 > 0)
      values1_gt_0.insert(value);

  std::set<int> gt_than_0_in_result;
  std::set_intersection(
      query_values_set.begin(), query_values_set.end(), values1_gt_0.begin(),
      values1_gt_0.end(),
      std::inserter(gt_than_0_in_result, gt_than_0_in_result.begin()));
  ASSERT_TRUE(gt_than_0_in_result.empty());
}

TEST_F(QueryFiltersFixture, test_multiply_filter_1) {
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
  less_than_expr_fnode->set_function_op(proto_msg::FunctionOP::EQUALS);
  auto *add_expr = less_than_expr_fnode->mutable_exprs()->Add();
  auto *add_expr_fnode = add_expr->mutable_function_node();
  add_expr_fnode->set_function_op(proto_msg::FunctionOP::MULTIPLY);
  auto *add_expr_first = add_expr_fnode->mutable_exprs()->Add();
  auto *add_expr_second = add_expr_fnode->mutable_exprs()->Add();

  auto *add_expr_first_var = add_expr_first->mutable_term_node();
  add_expr_first_var->set_term_value("?x");
  add_expr_first_var->set_term_type(proto_msg::TermType::VARIABLE);
  auto *add_expr_second_val = add_expr_second->mutable_term_node();
  add_expr_second_val->set_term_value("\"0\"^^xsd:integer");
  add_expr_second_val->set_term_type(proto_msg::TermType::LITERAL);

  auto *second_expr_lt = less_than_expr_fnode->mutable_exprs()->Add();
  auto *second_expr_lt_term = second_expr_lt->mutable_term_node();
  second_expr_lt_term->set_term_value("\"0\"^^xsd:integer");
  second_expr_lt_term->set_term_type(proto_msg::TermType::LITERAL);

  auto result = cache->run_query(tree);

  auto query_values_set = get_values_from_result_table(result.table(), *pcm);

  ASSERT_GE(values1.size(), query_values_set.size());
  ASSERT_EQ(values1, query_values_set);
}

TEST_F(QueryFiltersFixture, test_divide_filter_1) {
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
  less_than_expr_fnode->set_function_op(proto_msg::FunctionOP::EQUALS);
  auto *add_expr = less_than_expr_fnode->mutable_exprs()->Add();
  auto *add_expr_fnode = add_expr->mutable_function_node();
  add_expr_fnode->set_function_op(proto_msg::FunctionOP::DIVIDE);
  auto *add_expr_first = add_expr_fnode->mutable_exprs()->Add();
  auto *add_expr_second = add_expr_fnode->mutable_exprs()->Add();

  auto *add_expr_first_var = add_expr_first->mutable_term_node();
  add_expr_first_var->set_term_value("?x");
  add_expr_first_var->set_term_type(proto_msg::TermType::VARIABLE);
  auto *add_expr_second_val = add_expr_second->mutable_term_node();
  add_expr_second_val->set_term_value("\"0\"^^xsd:integer");
  add_expr_second_val->set_term_type(proto_msg::TermType::LITERAL);

  auto *second_expr_lt = less_than_expr_fnode->mutable_exprs()->Add();
  auto *second_expr_lt_term = second_expr_lt->mutable_term_node();
  second_expr_lt_term->set_term_value("\"0\"^^xsd:integer");
  second_expr_lt_term->set_term_type(proto_msg::TermType::LITERAL);

  auto result = cache->run_query(tree);

  auto query_values_set = get_values_from_result_table(result.table(), *pcm);

  ASSERT_TRUE(query_values_set.empty());
}

TEST_F(QueryFiltersFixture, test_divide_filter_2) {
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
  add_expr_fnode->set_function_op(proto_msg::FunctionOP::DIVIDE);
  auto *add_expr_first = add_expr_fnode->mutable_exprs()->Add();
  auto *add_expr_second = add_expr_fnode->mutable_exprs()->Add();

  auto *add_expr_first_var = add_expr_first->mutable_term_node();
  add_expr_first_var->set_term_value("?x");
  add_expr_first_var->set_term_type(proto_msg::TermType::VARIABLE);
  auto *add_expr_second_val = add_expr_second->mutable_term_node();
  add_expr_second_val->set_term_value("\"-1\"^^xsd:integer");
  add_expr_second_val->set_term_type(proto_msg::TermType::LITERAL);

  auto *second_expr_lt = less_than_expr_fnode->mutable_exprs()->Add();
  auto *second_expr_lt_term = second_expr_lt->mutable_term_node();
  second_expr_lt_term->set_term_value("\"0\"^^xsd:integer");
  second_expr_lt_term->set_term_type(proto_msg::TermType::LITERAL);

  auto result = cache->run_query(tree);

  auto query_values_set = get_values_from_result_table(result.table(), *pcm);

  std::set<int> greater_than_zero;
  for (auto value : values1)
    if (value > 0)
      greater_than_zero.insert(value);

  ASSERT_EQ(greater_than_zero, query_values_set);
}

TEST_F(QueryFiltersFixture, test_is_iri_1) {
  proto_msg::SparqlTree tree;
  auto *distinct_node = tree.mutable_root()->mutable_distinct_node();
  auto *project_node =
      distinct_node->mutable_sub_node()->mutable_project_node();
  project_node->add_vars("?x");
  auto *filter_node = project_node->mutable_sub_op()->mutable_filter_node();
  auto *bgp_node = filter_node->mutable_node()->mutable_bgp_node();
  auto *first_triple = bgp_node->mutable_triple()->Add();
  auto *first_subject = first_triple->mutable_subject();
  auto *first_predicate = first_triple->mutable_predicate();
  auto *first_object = first_triple->mutable_object();
  first_subject->set_term_value("?x");
  first_subject->set_term_type(proto_msg::TermType::VARIABLE);
  first_predicate->set_term_value("<has_integer>");
  first_predicate->set_term_type(proto_msg::TermType::IRI);
  first_object->set_term_value("?y");
  first_object->set_term_type(proto_msg::TermType::VARIABLE);

  auto *is_iri_expr = filter_node->mutable_exprs()->Add();
  auto *is_iri_expr_fnode = is_iri_expr->mutable_function_node();
  is_iri_expr_fnode->set_function_op(proto_msg::FunctionOP::IS_IRI);
  auto *term_expr = is_iri_expr_fnode->mutable_exprs()->Add();
  auto *term_expr_node = term_expr->mutable_term_node();
  term_expr_node->set_term_value("?x");
  term_expr_node->set_term_type(proto_msg::TermType::VARIABLE);

  auto result = cache->run_query(tree);

  auto query_values_set =
      get_string_values_from_result_table(result.table(), *pcm);

  ASSERT_TRUE(query_values_set.find("<some_integer_ref1>") !=
              query_values_set.end());
}

TEST_F(QueryFiltersFixture, test_is_blank_1) {
  proto_msg::SparqlTree tree;
  auto *distinct_node = tree.mutable_root()->mutable_distinct_node();
  auto *project_node =
      distinct_node->mutable_sub_node()->mutable_project_node();
  project_node->add_vars("?x");
  auto *filter_node = project_node->mutable_sub_op()->mutable_filter_node();
  auto *bgp_node = filter_node->mutable_node()->mutable_bgp_node();
  auto *first_triple = bgp_node->mutable_triple()->Add();
  auto *first_subject = first_triple->mutable_subject();
  auto *first_predicate = first_triple->mutable_predicate();
  auto *first_object = first_triple->mutable_object();
  first_subject->set_term_value("?y");
  first_subject->set_term_type(proto_msg::TermType::VARIABLE);
  first_predicate->set_term_value("<has_blank>");
  first_predicate->set_term_type(proto_msg::TermType::IRI);
  first_object->set_term_value("?x");
  first_object->set_term_type(proto_msg::TermType::VARIABLE);

  auto *is_iri_expr = filter_node->mutable_exprs()->Add();
  auto *is_iri_expr_fnode = is_iri_expr->mutable_function_node();
  is_iri_expr_fnode->set_function_op(proto_msg::FunctionOP::IS_BLANK);
  auto *term_expr = is_iri_expr_fnode->mutable_exprs()->Add();
  auto *term_expr_node = term_expr->mutable_term_node();
  term_expr_node->set_term_value("?x");
  term_expr_node->set_term_type(proto_msg::TermType::VARIABLE);

  auto result = cache->run_query(tree);

  auto query_values_set =
      get_string_values_from_result_table(result.table(), *pcm);

  ASSERT_TRUE(query_values_set.find("_:someBlankStuff") !=
              query_values_set.end());
}

TEST_F(QueryFiltersFixture, test_is_literal_1) {
  proto_msg::SparqlTree tree;
  auto *distinct_node = tree.mutable_root()->mutable_distinct_node();
  auto *project_node =
      distinct_node->mutable_sub_node()->mutable_project_node();
  project_node->add_vars("?x");
  auto *filter_node = project_node->mutable_sub_op()->mutable_filter_node();
  auto *bgp_node = filter_node->mutable_node()->mutable_bgp_node();
  auto *first_triple = bgp_node->mutable_triple()->Add();
  auto *first_subject = first_triple->mutable_subject();
  auto *first_predicate = first_triple->mutable_predicate();
  auto *first_object = first_triple->mutable_object();
  first_subject->set_term_value("?y");
  first_subject->set_term_type(proto_msg::TermType::VARIABLE);
  first_predicate->set_term_value("<has_literal>");
  first_predicate->set_term_type(proto_msg::TermType::IRI);
  first_object->set_term_value("?x");
  first_object->set_term_type(proto_msg::TermType::VARIABLE);

  auto *is_iri_expr = filter_node->mutable_exprs()->Add();
  auto *is_iri_expr_fnode = is_iri_expr->mutable_function_node();
  is_iri_expr_fnode->set_function_op(proto_msg::FunctionOP::IS_LITERAL);
  auto *term_expr = is_iri_expr_fnode->mutable_exprs()->Add();
  auto *term_expr_node = term_expr->mutable_term_node();
  term_expr_node->set_term_value("?x");
  term_expr_node->set_term_type(proto_msg::TermType::VARIABLE);

  auto result = cache->run_query(tree);

  auto query_values_set =
      get_string_values_from_result_table(result.table(), *pcm);
  ASSERT_TRUE(query_values_set.find("\"someLiteralStuff\"") !=
              query_values_set.end());
}

TEST_F(QueryFiltersFixture, test_is_numeric_1) {
  proto_msg::SparqlTree tree;
  auto *distinct_node = tree.mutable_root()->mutable_distinct_node();
  auto *project_node =
      distinct_node->mutable_sub_node()->mutable_project_node();
  project_node->add_vars("?x");
  auto *filter_node = project_node->mutable_sub_op()->mutable_filter_node();
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

  auto *is_iri_expr = filter_node->mutable_exprs()->Add();
  auto *is_iri_expr_fnode = is_iri_expr->mutable_function_node();
  is_iri_expr_fnode->set_function_op(proto_msg::FunctionOP::IS_NUMERIC);
  auto *term_expr = is_iri_expr_fnode->mutable_exprs()->Add();
  auto *term_expr_node = term_expr->mutable_term_node();
  term_expr_node->set_term_value("?x");
  term_expr_node->set_term_type(proto_msg::TermType::VARIABLE);

  auto result = cache->run_query(tree);

  auto query_values_set = get_values_from_result_table(result.table(), *pcm);

  ASSERT_EQ(query_values_set, values1);
}

TEST_F(QueryFiltersFixture, test_coalesce_1) {
  proto_msg::SparqlTree tree;
  auto *distinct_node = tree.mutable_root()->mutable_distinct_node();
  auto *project_node =
      distinct_node->mutable_sub_node()->mutable_project_node();
  project_node->add_vars("?x");
  auto *filter_node = project_node->mutable_sub_op()->mutable_filter_node();
  auto *bgp_node = filter_node->mutable_node()->mutable_bgp_node();
  auto *first_triple = bgp_node->mutable_triple()->Add();
  auto *first_subject = first_triple->mutable_subject();
  auto *first_predicate = first_triple->mutable_predicate();
  auto *first_object = first_triple->mutable_object();
  first_subject->set_term_value("?x");
  first_subject->set_term_type(proto_msg::TermType::VARIABLE);
  first_predicate->set_term_value("<has_integer>");
  first_predicate->set_term_type(proto_msg::TermType::IRI);
  first_object->set_term_value("?y");
  first_object->set_term_type(proto_msg::TermType::VARIABLE);

  auto *coalesce_expr = filter_node->mutable_exprs()->Add();
  auto *coalesce_fnode = coalesce_expr->mutable_function_node();
  coalesce_fnode->set_function_op(proto_msg::FunctionOP::COALESCE);
  auto *div_zero_node = coalesce_fnode->mutable_exprs()->Add();
  auto *div_zero_fnode = div_zero_node->mutable_function_node();
  div_zero_fnode->set_function_op(proto_msg::FunctionOP::DIVIDE);
  auto *div_zero_first_expr = div_zero_fnode->add_exprs();
  auto *div_zero_first_term = div_zero_first_expr->mutable_term_node();
  div_zero_first_term->set_term_value("?x");
  div_zero_first_term->set_term_type(proto_msg::TermType::VARIABLE);
  auto *div_zero_second_expr = div_zero_fnode->add_exprs();
  auto *div_zero_second_term = div_zero_second_expr->mutable_term_node();
  div_zero_second_term->set_term_value("\"0\"^^xsd:integer");
  div_zero_second_term->set_term_type(proto_msg::TermType::LITERAL);

  auto *is_iri_expr = coalesce_fnode->mutable_exprs()->Add();
  auto *is_iri_expr_fnode = is_iri_expr->mutable_function_node();
  is_iri_expr_fnode->set_function_op(proto_msg::FunctionOP::IS_IRI);
  auto *term_expr = is_iri_expr_fnode->mutable_exprs()->Add();
  auto *term_expr_node = term_expr->mutable_term_node();
  term_expr_node->set_term_value("?x");
  term_expr_node->set_term_type(proto_msg::TermType::VARIABLE);

  auto result = cache->run_query(tree);

  auto query_values_set =
      get_string_values_from_result_table(result.table(), *pcm);

  ASSERT_TRUE(query_values_set.find("<some_integer_ref1>") !=
              query_values_set.end());
}

TEST_F(QueryFiltersFixture, test_datatype_1) {
  proto_msg::SparqlTree tree;
  auto *distinct_node = tree.mutable_root()->mutable_distinct_node();
  auto *project_node =
      distinct_node->mutable_sub_node()->mutable_project_node();
  project_node->add_vars("?x");
  auto *filter_node = project_node->mutable_sub_op()->mutable_filter_node();
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

  auto *equals_expr = filter_node->mutable_exprs()->Add();
  auto *equals_fnode = equals_expr->mutable_function_node();
  equals_fnode->set_function_op(proto_msg::FunctionOP::EQUALS);

  auto *data_type_expr = equals_fnode->mutable_exprs()->Add();
  auto *data_type_fnode = data_type_expr->mutable_function_node();
  data_type_fnode->set_function_op(proto_msg::FunctionOP::DATA_TYPE);
  auto *term_expr = data_type_fnode->mutable_exprs()->Add();
  auto *term = term_expr->mutable_term_node();
  term->set_term_value("?x");
  term->set_term_type(proto_msg::TermType::VARIABLE);

  auto *rhs_expr = equals_fnode->mutable_exprs()->Add();
  auto *rhs_term = rhs_expr->mutable_term_node();
  rhs_term->set_term_type(proto_msg::TermType::IRI);
  rhs_term->set_term_value("xsd:integer");

  auto result = cache->run_query(tree);

  auto query_values_set =
      get_string_values_from_result_table(result.table(), *pcm);

  // ASSERT_TRUE(query_values_set.find("<some_integer_ref1>") !=
  // query_values_set.end());
}

static int select_date_info_by_op(int i, proto_msg::FunctionOP function_op) {
  switch (function_op) {
  case proto_msg::FunctionOP::DATE_TIME_HOURS:
    return QueryFiltersFixture::date_infos_expected[i].hour;
  case proto_msg::FunctionOP::DATE_TIME_DAY:
    return QueryFiltersFixture::date_infos_expected[i].day;
  case proto_msg::FunctionOP::DATE_TIME_MINUTES:
    return QueryFiltersFixture::date_infos_expected[i].minute;
  case proto_msg::FunctionOP::DATE_TIME_SECONDS:
    return QueryFiltersFixture::date_infos_expected[i].second;
  case proto_msg::FunctionOP::DATE_TIME_MONTH:
    return QueryFiltersFixture::date_infos_expected[i].month;
  case proto_msg::FunctionOP::DATE_TIME_YEAR:
    return QueryFiltersFixture::date_infos_expected[i].year;
  default:
    return -1;
  }
  return -1;
}

static void run_datetime_part_test(proto_msg::FunctionOP function_op, int value,
                                   QueryResult &out_result) {
  proto_msg::SparqlTree tree;
  auto *distinct_node = tree.mutable_root()->mutable_distinct_node();
  auto *project_node =
      distinct_node->mutable_sub_node()->mutable_project_node();
  project_node->add_vars("?x");
  auto *filter_node = project_node->mutable_sub_op()->mutable_filter_node();
  auto *bgp_node = filter_node->mutable_node()->mutable_bgp_node();
  auto *first_triple = bgp_node->mutable_triple()->Add();
  auto *first_subject = first_triple->mutable_subject();
  auto *first_predicate = first_triple->mutable_predicate();
  auto *first_object = first_triple->mutable_object();
  first_subject->set_term_value("?y");
  first_subject->set_term_type(proto_msg::TermType::VARIABLE);
  first_predicate->set_term_value("<has_date>");
  first_predicate->set_term_type(proto_msg::TermType::IRI);
  first_object->set_term_value("?x");
  first_object->set_term_type(proto_msg::TermType::VARIABLE);

  auto *equals_expr = filter_node->mutable_exprs()->Add();
  auto *equals_fnode = equals_expr->mutable_function_node();
  equals_fnode->set_function_op(proto_msg::FunctionOP::EQUALS);

  auto *data_type_expr = equals_fnode->mutable_exprs()->Add();
  auto *data_type_fnode = data_type_expr->mutable_function_node();
  data_type_fnode->set_function_op(function_op);
  auto *term_expr = data_type_fnode->mutable_exprs()->Add();
  auto *term = term_expr->mutable_term_node();
  term->set_term_value("?x");
  term->set_term_type(proto_msg::TermType::VARIABLE);

  auto *rhs_expr = equals_fnode->mutable_exprs()->Add();
  auto *rhs_term = rhs_expr->mutable_term_node();
  rhs_term->set_term_type(proto_msg::TermType::LITERAL);
  rhs_term->set_term_value("\"" + std::to_string(value) + "\"^^xsd:integer");

  auto result = QueryFiltersFixture::cache->run_query(tree);

  auto query_values_set = get_string_values_from_result_table(
      result.table(), *QueryFiltersFixture::pcm);

  std::set<int> matching_indices_in_result;

  for (const auto &single_result_str : query_values_set) {
    auto literal_data =
        StringHandlingUtil::extract_literal_data_from_string(single_result_str);
    auto it_result =
        std::find(QueryFiltersFixture::dates.begin(),
                  QueryFiltersFixture::dates.end(), literal_data.value);
    if (it_result != QueryFiltersFixture::dates.end()) {
      size_t matching_index = it_result - QueryFiltersFixture::dates.begin();
      matching_indices_in_result.insert(matching_index);
    }
  }

  std::set<int> expected_dates_indices_result;

  for (size_t i = 0; i < QueryFiltersFixture::date_infos_expected.size(); i++) {
    if (select_date_info_by_op(i, function_op) == value) {
      expected_dates_indices_result.insert(i);
    }
  }

  ASSERT_EQ(expected_dates_indices_result, matching_indices_in_result);

  out_result = std::move(result);
}

TEST_F(QueryFiltersFixture, test_datetime_day_1) {
  QueryResult result;
  run_datetime_part_test(proto_msg::FunctionOP::DATE_TIME_DAY, 2, result);
}

TEST_F(QueryFiltersFixture, test_datetime_hours_1) {
  QueryResult result;
  std::vector<int> hours = {19, 11};
  for (int hour : hours) {
    run_datetime_part_test(proto_msg::FunctionOP::DATE_TIME_HOURS, hour,
                           result);
    ASSERT_GT(result.table().data.size(), 0);
  }
}

TEST_F(QueryFiltersFixture, test_datetime_month_1) {
  QueryResult result;
  std::vector<int> months = {1, 5, 12};
  for (int month : months) {
    run_datetime_part_test(proto_msg::FunctionOP::DATE_TIME_MONTH, month,
                           result);
    ASSERT_GT(result.table().data.size(), 0);
  }
}

TEST_F(QueryFiltersFixture, test_datetime_year_1) {
  QueryResult result;
  for (int i = 2021; i <= 2026; i++) {
    run_datetime_part_test(proto_msg::FunctionOP::DATE_TIME_YEAR, i, result);
    ASSERT_GT(result.table().data.size(), 0);
  }
}

TEST_F(QueryFiltersFixture, test_datetime_minutes_1) {
  QueryResult result;
  std::vector<int> minutes = {50, 20};
  for (int minute : minutes) {
    run_datetime_part_test(proto_msg::FunctionOP::DATE_TIME_MINUTES, minute,
                           result);
    ASSERT_GT(result.table().data.size(), 0);
  }
}

TEST_F(QueryFiltersFixture, test_datetime_seconds_1) {
  QueryResult result;
  std::vector<int> seconds = {54, 34, 44};
  for (int second : seconds) {
    run_datetime_part_test(proto_msg::FunctionOP::DATE_TIME_SECONDS, second,
                           result);
    ASSERT_GT(result.table().data.size(), 0);
  }
}

static void run_datetime_tz_query(const std::string &offset_str,
                                  int expected_offset_sign,
                                  int expected_offset_hour,
                                  int expected_offset_minute,
                                  std::set<std::string> &result_values_set) {
  proto_msg::SparqlTree tree;
  auto *distinct_node = tree.mutable_root()->mutable_distinct_node();
  auto *project_node =
      distinct_node->mutable_sub_node()->mutable_project_node();
  project_node->add_vars("?x");
  auto *filter_node = project_node->mutable_sub_op()->mutable_filter_node();
  auto *bgp_node = filter_node->mutable_node()->mutable_bgp_node();
  auto *first_triple = bgp_node->mutable_triple()->Add();
  auto *first_subject = first_triple->mutable_subject();
  auto *first_predicate = first_triple->mutable_predicate();
  auto *first_object = first_triple->mutable_object();
  first_subject->set_term_value("?y");
  first_subject->set_term_type(proto_msg::TermType::VARIABLE);
  first_predicate->set_term_value("<has_date>");
  first_predicate->set_term_type(proto_msg::TermType::IRI);
  first_object->set_term_value("?x");
  first_object->set_term_type(proto_msg::TermType::VARIABLE);

  auto *equals_expr = filter_node->mutable_exprs()->Add();
  auto *equals_fnode = equals_expr->mutable_function_node();
  equals_fnode->set_function_op(proto_msg::FunctionOP::EQUALS);

  auto *data_type_expr = equals_fnode->mutable_exprs()->Add();
  auto *data_type_fnode = data_type_expr->mutable_function_node();
  data_type_fnode->set_function_op(proto_msg::FunctionOP::DATE_TIME_TZ);
  auto *term_expr = data_type_fnode->mutable_exprs()->Add();
  auto *term = term_expr->mutable_term_node();
  term->set_term_value("?x");
  term->set_term_type(proto_msg::TermType::VARIABLE);

  auto *rhs_expr = equals_fnode->mutable_exprs()->Add();
  auto *rhs_term = rhs_expr->mutable_term_node();
  rhs_term->set_term_type(proto_msg::TermType::LITERAL);
  rhs_term->set_term_value("\"" + offset_str + "\"");

  auto result = QueryFiltersFixture::cache->run_query(tree);

  auto query_values_set = get_string_values_from_result_table(
      result.table(), *QueryFiltersFixture::pcm);

  std::set<int> expected_set;
  std::set<int> matched_set;
  for (size_t i = 0; i < QueryFiltersFixture::date_infos_expected.size(); i++) {
    const auto &date_info = QueryFiltersFixture::date_infos_expected[i];
    if (date_info.offset_hour == expected_offset_hour &&
        date_info.offset_minute == expected_offset_minute &&
        date_info.offset_sign * expected_offset_sign > 0) {
      expected_set.insert(i);
    }
    if (query_values_set.find("\"" + QueryFiltersFixture::dates[i] +
                              "\"^^xsd:dateTime") != query_values_set.end()) {
      matched_set.insert(i);
    }
  }
  ASSERT_EQ(matched_set, expected_set);
  result_values_set = std::move(query_values_set);
}

TEST_F(QueryFiltersFixture, test_datetime_tz_1) {
  std::set<std::string> result_values_set;
  run_datetime_tz_query("Z", 1, 0, 0, result_values_set);
  ASSERT_GE(result_values_set.size(), 6);
}

TEST_F(QueryFiltersFixture, test_datetime_tz_2) {
  std::set<std::string> result_values_set;
  run_datetime_tz_query("-05:31", -1, 5, 31, result_values_set);
  ASSERT_GE(result_values_set.size(), 1);
}
TEST_F(QueryFiltersFixture, test_datetime_tz_3) {
  std::set<std::string> result_values_set;
  run_datetime_tz_query("+09:31", 1, 9, 31, result_values_set);
  ASSERT_GE(result_values_set.size(), 1);
}

static void run_query_digest_hash_test(proto_msg::FunctionOP function_op,
                                       const std::string &expected_hash) {
  proto_msg::SparqlTree tree;
  auto *distinct_node = tree.mutable_root()->mutable_distinct_node();
  auto *project_node =
      distinct_node->mutable_sub_node()->mutable_project_node();
  project_node->add_vars("?x");
  auto *filter_node = project_node->mutable_sub_op()->mutable_filter_node();
  auto *bgp_node = filter_node->mutable_node()->mutable_bgp_node();
  auto *first_triple = bgp_node->mutable_triple()->Add();
  auto *first_subject = first_triple->mutable_subject();
  auto *first_predicate = first_triple->mutable_predicate();
  auto *first_object = first_triple->mutable_object();
  first_subject->set_term_value("?y");
  first_subject->set_term_type(proto_msg::TermType::VARIABLE);
  first_predicate->set_term_value("<has_date>");
  first_predicate->set_term_type(proto_msg::TermType::IRI);
  first_object->set_term_value("?x");
  first_object->set_term_type(proto_msg::TermType::VARIABLE);

  auto *equals_expr = filter_node->mutable_exprs()->Add();
  auto *equals_fnode = equals_expr->mutable_function_node();
  equals_fnode->set_function_op(proto_msg::FunctionOP::EQUALS);

  auto *data_type_expr = equals_fnode->mutable_exprs()->Add();
  auto *data_type_fnode = data_type_expr->mutable_function_node();
  data_type_fnode->set_function_op(function_op);
  auto *term_expr = data_type_fnode->mutable_exprs()->Add();
  auto *term = term_expr->mutable_term_node();
  term->set_term_value("?x");
  term->set_term_type(proto_msg::TermType::VARIABLE);

  auto *rhs_expr = equals_fnode->mutable_exprs()->Add();
  auto *rhs_term = rhs_expr->mutable_term_node();
  rhs_term->set_term_type(proto_msg::TermType::LITERAL);
  rhs_term->set_term_value("\"" + expected_hash + "\"");

  auto result = QueryFiltersFixture::cache->run_query(tree);

  auto query_values_set = get_string_values_from_result_table(
      result.table(), *QueryFiltersFixture::pcm);

  ASSERT_GT(query_values_set.size(), 0);

  std::set<std::string> hashed_dates;
  std::string (*hash_fun)(const std::string &);
  switch (function_op) {
  case proto_msg::FunctionOP::DIGEST_MD5:
    hash_fun = md5_human_readable_lowercase;
    break;
  case proto_msg::FunctionOP::DIGEST_SHA1:
    hash_fun = sha1_human_readable_lowercase;
    break;
  case proto_msg::FunctionOP::DIGEST_SHA224:
    hash_fun = sha224_human_readable_lowercase;
    break;
  case proto_msg::FunctionOP::DIGEST_SHA256:
    hash_fun = sha256_human_readable_lowercase;
    break;
  case proto_msg::FunctionOP::DIGEST_SHA384:
    hash_fun = sha384_human_readable_lowercase;
    break;
  case proto_msg::FunctionOP::DIGEST_SHA512:
    hash_fun = sha512_human_readable_lowercase;
    break;
  default:
    throw std::runtime_error("expected a digest hash function op");
  }

  for (const auto &date : QueryFiltersFixture::dates) {
    hashed_dates.insert(hash_fun(date));
  }

  for (const auto &result_date : query_values_set) {
    auto literal_data =
        StringHandlingUtil::extract_literal_data_from_string(result_date);
    ASSERT_NE(hashed_dates.find(hash_fun(literal_data.value)),
              hashed_dates.end());
  }
}

TEST_F(QueryFiltersFixture, test_md5_eval_1) {
  for (const auto &date : dates) {
    run_query_digest_hash_test(proto_msg::FunctionOP::DIGEST_MD5,
                               md5_human_readable_lowercase(date));
  }
}

TEST_F(QueryFiltersFixture, test_sha1_eval_1) {
  for (const auto &date : dates) {
    run_query_digest_hash_test(proto_msg::FunctionOP::DIGEST_SHA1,
                               sha1_human_readable_lowercase(date));
  }
}

TEST_F(QueryFiltersFixture, test_sha224_eval_1) {
  for (const auto &date : dates) {
    run_query_digest_hash_test(proto_msg::FunctionOP::DIGEST_SHA224,
                               sha224_human_readable_lowercase(date));
  }
}

TEST_F(QueryFiltersFixture, test_sha256_eval_1) {
  for (const auto &date : dates) {
    run_query_digest_hash_test(proto_msg::FunctionOP::DIGEST_SHA256,
                               sha256_human_readable_lowercase(date));
  }
}

TEST_F(QueryFiltersFixture, test_sha384_eval_1) {
  for (const auto &date : dates) {
    run_query_digest_hash_test(proto_msg::FunctionOP::DIGEST_SHA384,
                               sha384_human_readable_lowercase(date));
  }
}

TEST_F(QueryFiltersFixture, test_sha512_eval_1) {
  for (const auto &date : dates) {
    run_query_digest_hash_test(proto_msg::FunctionOP::DIGEST_SHA512,
                               sha512_human_readable_lowercase(date));
  }
}

TEST_F(QueryFiltersFixture, test_gt_eval_1) {
  proto_msg::SparqlTree tree;
  auto *distinct_node = tree.mutable_root()->mutable_distinct_node();
  auto *project_node =
      distinct_node->mutable_sub_node()->mutable_project_node();
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
  less_than_expr_fnode->set_function_op(proto_msg::FunctionOP::GREATER_THAN);
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
  second_expr_lt_term->set_term_value("\"50\"^^xsd:integer");
  second_expr_lt_term->set_term_type(proto_msg::TermType::LITERAL);

  auto result = cache->run_query(tree);

  auto query_values_set = get_values_from_result_table(result.table(), *pcm);

  std::set<int> expected_values;
  for (auto value : QueryFiltersFixture::values1) {
    if (value + 1 > 50)
      expected_values.insert(value);
  }

  ASSERT_EQ(query_values_set, expected_values);
}

TEST_F(QueryFiltersFixture, test_gte_eval_1) {
  proto_msg::SparqlTree tree;
  auto *distinct_node = tree.mutable_root()->mutable_distinct_node();
  auto *project_node =
      distinct_node->mutable_sub_node()->mutable_project_node();
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
  less_than_expr_fnode->set_function_op(
      proto_msg::FunctionOP::GREATER_THAN_OR_EQUAL);
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
  second_expr_lt_term->set_term_value("\"50\"^^xsd:integer");
  second_expr_lt_term->set_term_type(proto_msg::TermType::LITERAL);

  auto result = cache->run_query(tree);

  auto query_values_set = get_values_from_result_table(result.table(), *pcm);

  std::set<int> expected_values;
  for (auto value : QueryFiltersFixture::values1) {
    if (value + 1 >= 50)
      expected_values.insert(value);
  }

  ASSERT_EQ(query_values_set, expected_values);
}

TEST_F(QueryFiltersFixture, test_lt_eval_1) {
  proto_msg::SparqlTree tree;
  auto *distinct_node = tree.mutable_root()->mutable_distinct_node();
  auto *project_node =
      distinct_node->mutable_sub_node()->mutable_project_node();
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
  second_expr_lt_term->set_term_value("\"50\"^^xsd:integer");
  second_expr_lt_term->set_term_type(proto_msg::TermType::LITERAL);

  auto result = cache->run_query(tree);

  auto query_values_set = get_values_from_result_table(result.table(), *pcm);

  std::set<int> expected_values;
  for (auto value : QueryFiltersFixture::values1) {
    if (value + 1 < 50)
      expected_values.insert(value);
  }

  ASSERT_EQ(query_values_set, expected_values);
}

TEST_F(QueryFiltersFixture, test_lte_eval_1) {
  proto_msg::SparqlTree tree;
  auto *distinct_node = tree.mutable_root()->mutable_distinct_node();
  auto *project_node =
      distinct_node->mutable_sub_node()->mutable_project_node();
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
  less_than_expr_fnode->set_function_op(
      proto_msg::FunctionOP::LESS_THAN_OR_EQUAL);
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
  second_expr_lt_term->set_term_value("\"50\"^^xsd:integer");
  second_expr_lt_term->set_term_type(proto_msg::TermType::LITERAL);

  auto result = cache->run_query(tree);

  auto query_values_set = get_values_from_result_table(result.table(), *pcm);

  std::set<int> expected_values;
  for (auto value : QueryFiltersFixture::values1) {
    if (value + 1 <= 50)
      expected_values.insert(value);
  }

  ASSERT_EQ(query_values_set, expected_values);
}

TEST_F(QueryFiltersFixture, test_logical_and_eval_1) {
  proto_msg::SparqlTree tree;
  auto *distinct_node = tree.mutable_root()->mutable_distinct_node();
  auto *project_node =
      distinct_node->mutable_sub_node()->mutable_project_node();
  project_node->add_vars("?x");
  auto *filter_node = project_node->mutable_sub_op()->mutable_filter_node();
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

  auto *logical_and_expr = filter_node->mutable_exprs()->Add();
  auto *logical_and_fnode = logical_and_expr->mutable_function_node();
  logical_and_fnode->set_function_op(proto_msg::FunctionOP::LOGICAL_AND);

  auto *gt_expr = logical_and_fnode->mutable_exprs()->Add();
  auto *gt_fnode = gt_expr->mutable_function_node();
  gt_fnode->set_function_op(proto_msg::FunctionOP::GREATER_THAN);
  auto *term_expr_left = gt_fnode->mutable_exprs()->Add();
  auto *term_left = term_expr_left->mutable_term_node();
  term_left->set_term_value("?x");
  term_left->set_term_type(proto_msg::TermType::VARIABLE);
  auto *term_expr_right = gt_fnode->mutable_exprs()->Add();
  auto *term_right = term_expr_right->mutable_term_node();
  term_right->set_term_value("\"0\"^^xsd:integer");
  term_right->set_term_type(proto_msg::TermType::LITERAL);

  auto *eq_expr = logical_and_fnode->mutable_exprs()->Add();
  auto *eq_fnode = eq_expr->mutable_function_node();
  eq_fnode->set_function_op(proto_msg::FunctionOP::EQUALS);

  auto *lhs_eq_expr = eq_fnode->mutable_exprs()->Add();
  auto *rhs_eq_expr = eq_fnode->mutable_exprs()->Add();

  lhs_eq_expr->mutable_term_node()->set_term_value("?x");
  lhs_eq_expr->mutable_term_node()->set_term_type(
      proto_msg::TermType::VARIABLE);

  auto *mult_fnode = rhs_eq_expr->mutable_function_node();
  mult_fnode->set_function_op(proto_msg::FunctionOP::MULTIPLY);
  auto *lhs_div_expr = mult_fnode->mutable_exprs()->Add();
  auto *rhs_div_expr = mult_fnode->mutable_exprs()->Add();
  auto *div_fnode = lhs_div_expr->mutable_function_node();
  div_fnode->set_function_op(proto_msg::FunctionOP::DIVIDE);

  rhs_div_expr->mutable_term_node()->set_term_value("\"2\"^^xsd:integer");
  rhs_div_expr->mutable_term_node()->set_term_type(
      proto_msg::TermType::LITERAL);

  auto *div_lhs_expr = div_fnode->mutable_exprs()->Add();
  auto *div_rhs_expr = div_fnode->mutable_exprs()->Add();

  div_lhs_expr->mutable_term_node()->set_term_type(
      proto_msg::TermType::VARIABLE);
  div_lhs_expr->mutable_term_node()->set_term_value("?x");

  div_rhs_expr->mutable_term_node()->set_term_type(
      proto_msg::TermType::LITERAL);
  div_rhs_expr->mutable_term_node()->set_term_value("\"2\"^^xsd:integer");

  auto result = QueryFiltersFixture::cache->run_query(tree);

  auto query_values_set =
      get_values_from_result_table(result.table(), *QueryFiltersFixture::pcm);

  std::set<int> expected_values;

  for (auto value : values1) {
    if (value > 0 && value % 2 == 0) {
      expected_values.insert(value);
    }
  }

  ASSERT_EQ(query_values_set, expected_values);
}

TEST_F(QueryFiltersFixture, test_logical_or_eval_1) {
  proto_msg::SparqlTree tree;
  auto *distinct_node = tree.mutable_root()->mutable_distinct_node();
  auto *project_node =
      distinct_node->mutable_sub_node()->mutable_project_node();
  project_node->add_vars("?x");
  auto *filter_node = project_node->mutable_sub_op()->mutable_filter_node();
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

  auto *logical_and_expr = filter_node->mutable_exprs()->Add();
  auto *logical_and_fnode = logical_and_expr->mutable_function_node();
  logical_and_fnode->set_function_op(proto_msg::FunctionOP::LOGICAL_OR);

  auto *gt_expr = logical_and_fnode->mutable_exprs()->Add();
  auto *gt_fnode = gt_expr->mutable_function_node();
  gt_fnode->set_function_op(proto_msg::FunctionOP::GREATER_THAN);
  auto *term_expr_left = gt_fnode->mutable_exprs()->Add();
  auto *term_left = term_expr_left->mutable_term_node();
  term_left->set_term_value("?x");
  term_left->set_term_type(proto_msg::TermType::VARIABLE);
  auto *term_expr_right = gt_fnode->mutable_exprs()->Add();
  auto *term_right = term_expr_right->mutable_term_node();
  term_right->set_term_value("\"0\"^^xsd:integer");
  term_right->set_term_type(proto_msg::TermType::LITERAL);

  auto *eq_expr = logical_and_fnode->mutable_exprs()->Add();
  auto *eq_fnode = eq_expr->mutable_function_node();
  eq_fnode->set_function_op(proto_msg::FunctionOP::EQUALS);

  auto *lhs_eq_expr = eq_fnode->mutable_exprs()->Add();
  auto *rhs_eq_expr = eq_fnode->mutable_exprs()->Add();

  lhs_eq_expr->mutable_term_node()->set_term_value("?x");
  lhs_eq_expr->mutable_term_node()->set_term_type(
      proto_msg::TermType::VARIABLE);

  auto *mult_fnode = rhs_eq_expr->mutable_function_node();
  mult_fnode->set_function_op(proto_msg::FunctionOP::MULTIPLY);
  auto *lhs_div_expr = mult_fnode->mutable_exprs()->Add();
  auto *rhs_div_expr = mult_fnode->mutable_exprs()->Add();
  auto *div_fnode = lhs_div_expr->mutable_function_node();
  div_fnode->set_function_op(proto_msg::FunctionOP::DIVIDE);

  rhs_div_expr->mutable_term_node()->set_term_value("\"2\"^^xsd:integer");
  rhs_div_expr->mutable_term_node()->set_term_type(
      proto_msg::TermType::LITERAL);

  auto *div_lhs_expr = div_fnode->mutable_exprs()->Add();
  auto *div_rhs_expr = div_fnode->mutable_exprs()->Add();

  div_lhs_expr->mutable_term_node()->set_term_type(
      proto_msg::TermType::VARIABLE);
  div_lhs_expr->mutable_term_node()->set_term_value("?x");

  div_rhs_expr->mutable_term_node()->set_term_type(
      proto_msg::TermType::LITERAL);
  div_rhs_expr->mutable_term_node()->set_term_value("\"2\"^^xsd:integer");

  auto result = QueryFiltersFixture::cache->run_query(tree);

  auto query_values_set =
      get_values_from_result_table(result.table(), *QueryFiltersFixture::pcm);

  std::set<int> expected_values;

  for (auto value : values1) {
    if (value > 0 || value % 2 == 0) {
      expected_values.insert(value);
    }
  }

  ASSERT_EQ(query_values_set, expected_values);
}

TEST_F(QueryFiltersFixture, test_logical_not_eval_1) {
  proto_msg::SparqlTree tree;
  auto *distinct_node = tree.mutable_root()->mutable_distinct_node();
  auto *project_node =
      distinct_node->mutable_sub_node()->mutable_project_node();
  project_node->add_vars("?x");
  auto *filter_node = project_node->mutable_sub_op()->mutable_filter_node();
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

  auto *logical_not_expr = filter_node->mutable_exprs()->Add();
  auto *logical_not_fnode = logical_not_expr->mutable_function_node();
  logical_not_fnode->set_function_op(proto_msg::FunctionOP::LOGICAL_NOT);

  auto *eq_expr = logical_not_fnode->mutable_exprs()->Add();
  auto *eq_fnode = eq_expr->mutable_function_node();
  eq_fnode->set_function_op(proto_msg::FunctionOP::EQUALS);

  auto *lhs_eq_expr = eq_fnode->mutable_exprs()->Add();
  auto *rhs_eq_expr = eq_fnode->mutable_exprs()->Add();

  lhs_eq_expr->mutable_term_node()->set_term_value("?x");
  lhs_eq_expr->mutable_term_node()->set_term_type(
      proto_msg::TermType::VARIABLE);

  auto *mult_fnode = rhs_eq_expr->mutable_function_node();
  mult_fnode->set_function_op(proto_msg::FunctionOP::MULTIPLY);
  auto *lhs_div_expr = mult_fnode->mutable_exprs()->Add();
  auto *rhs_div_expr = mult_fnode->mutable_exprs()->Add();
  auto *div_fnode = lhs_div_expr->mutable_function_node();
  div_fnode->set_function_op(proto_msg::FunctionOP::DIVIDE);

  rhs_div_expr->mutable_term_node()->set_term_value("\"2\"^^xsd:integer");
  rhs_div_expr->mutable_term_node()->set_term_type(
      proto_msg::TermType::LITERAL);

  auto *div_lhs_expr = div_fnode->mutable_exprs()->Add();
  auto *div_rhs_expr = div_fnode->mutable_exprs()->Add();

  div_lhs_expr->mutable_term_node()->set_term_type(
      proto_msg::TermType::VARIABLE);
  div_lhs_expr->mutable_term_node()->set_term_value("?x");

  div_rhs_expr->mutable_term_node()->set_term_type(
      proto_msg::TermType::LITERAL);
  div_rhs_expr->mutable_term_node()->set_term_value("\"2\"^^xsd:integer");

  auto result = QueryFiltersFixture::cache->run_query(tree);

  auto query_values_set =
      get_values_from_result_table(result.table(), *QueryFiltersFixture::pcm);

  std::set<int> expected_values;

  for (auto value : values1) {
    if (value % 2 != 0) {
      expected_values.insert(value);
    }
  }

  ASSERT_EQ(query_values_set, expected_values);
}

TEST_F(QueryFiltersFixture, test_not_equals_eval_1) {
  proto_msg::SparqlTree tree;
  auto *distinct_node = tree.mutable_root()->mutable_distinct_node();
  auto *project_node =
      distinct_node->mutable_sub_node()->mutable_project_node();
  project_node->add_vars("?x");
  auto *filter_node = project_node->mutable_sub_op()->mutable_filter_node();
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

  auto *eq_expr = filter_node->mutable_exprs()->Add();
  auto *eq_fnode = eq_expr->mutable_function_node();
  eq_fnode->set_function_op(proto_msg::FunctionOP::NOT_EQUALS);

  auto *lhs_eq_expr = eq_fnode->mutable_exprs()->Add();
  auto *rhs_eq_expr = eq_fnode->mutable_exprs()->Add();

  lhs_eq_expr->mutable_term_node()->set_term_value("?x");
  lhs_eq_expr->mutable_term_node()->set_term_type(
      proto_msg::TermType::VARIABLE);

  auto *mult_fnode = rhs_eq_expr->mutable_function_node();
  mult_fnode->set_function_op(proto_msg::FunctionOP::MULTIPLY);
  auto *lhs_div_expr = mult_fnode->mutable_exprs()->Add();
  auto *rhs_div_expr = mult_fnode->mutable_exprs()->Add();
  auto *div_fnode = lhs_div_expr->mutable_function_node();
  div_fnode->set_function_op(proto_msg::FunctionOP::DIVIDE);

  rhs_div_expr->mutable_term_node()->set_term_value("\"2\"^^xsd:integer");
  rhs_div_expr->mutable_term_node()->set_term_type(
      proto_msg::TermType::LITERAL);

  auto *div_lhs_expr = div_fnode->mutable_exprs()->Add();
  auto *div_rhs_expr = div_fnode->mutable_exprs()->Add();

  div_lhs_expr->mutable_term_node()->set_term_type(
      proto_msg::TermType::VARIABLE);
  div_lhs_expr->mutable_term_node()->set_term_value("?x");

  div_rhs_expr->mutable_term_node()->set_term_type(
      proto_msg::TermType::LITERAL);
  div_rhs_expr->mutable_term_node()->set_term_value("\"2\"^^xsd:integer");

  auto result = QueryFiltersFixture::cache->run_query(tree);

  auto query_values_set =
      get_values_from_result_table(result.table(), *QueryFiltersFixture::pcm);

  std::set<int> expected_values;

  for (auto value : values1) {
    if (value % 2 != 0) {
      expected_values.insert(value);
    }
  }

  ASSERT_EQ(query_values_set, expected_values);
}

TEST_F(QueryFiltersFixture, test_num_abs_eval_1) {
  proto_msg::SparqlTree tree;
  auto *distinct_node = tree.mutable_root()->mutable_distinct_node();
  auto *project_node =
      distinct_node->mutable_sub_node()->mutable_project_node();
  project_node->add_vars("?x");
  auto *filter_node = project_node->mutable_sub_op()->mutable_filter_node();
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

  auto *eq_expr = filter_node->mutable_exprs()->Add();
  auto *eq_fnode = eq_expr->mutable_function_node();
  eq_fnode->set_function_op(proto_msg::FunctionOP::LESS_THAN_OR_EQUAL);

  auto *abs_expr = eq_fnode->mutable_exprs()->Add();

  auto *abs_fnode = abs_expr->mutable_function_node();
  abs_fnode->set_function_op(proto_msg::FunctionOP::NUM_ABS);
  auto *abs_term_expr = abs_fnode->mutable_exprs()->Add();
  auto *abs_term = abs_term_expr->mutable_term_node();
  abs_term->set_term_value("?x");
  abs_term->set_term_type(proto_msg::VARIABLE);

  auto *lhs_eq_expr = eq_fnode->mutable_exprs()->Add();
  lhs_eq_expr->mutable_term_node()->set_term_type(proto_msg::TermType::LITERAL);
  lhs_eq_expr->mutable_term_node()->set_term_value("\"50\"^^xsd:integer");

  auto result = QueryFiltersFixture::cache->run_query(tree);

  auto query_values_set =
      get_values_from_result_table(result.table(), *QueryFiltersFixture::pcm);

  std::set<int> expected_values;

  for (auto value : values1) {
    if (std::abs(value) <= 50)
      expected_values.insert(value);
  }

  ASSERT_EQ(query_values_set, expected_values);
}

TEST_F(QueryFiltersFixture, test_num_ceiling_floor_eval_1) {
  proto_msg::SparqlTree tree;
  auto *distinct_node = tree.mutable_root()->mutable_distinct_node();
  auto *project_node =
      distinct_node->mutable_sub_node()->mutable_project_node();
  project_node->add_vars("?x");
  auto *filter_node = project_node->mutable_sub_op()->mutable_filter_node();
  auto *bgp_node = filter_node->mutable_node()->mutable_bgp_node();
  auto *first_triple = bgp_node->mutable_triple()->Add();
  auto *first_subject = first_triple->mutable_subject();
  auto *first_predicate = first_triple->mutable_predicate();
  auto *first_object = first_triple->mutable_object();
  first_subject->set_term_value("?y");
  first_subject->set_term_type(proto_msg::TermType::VARIABLE);
  first_predicate->set_term_value("<has_double>");
  first_predicate->set_term_type(proto_msg::TermType::IRI);
  first_object->set_term_value("?x");
  first_object->set_term_type(proto_msg::TermType::VARIABLE);

  auto *eq_expr = filter_node->mutable_exprs()->Add();
  auto *eq_fnode = eq_expr->mutable_function_node();
  eq_fnode->set_function_op(proto_msg::FunctionOP::EQUALS);

  auto *lhs_eq_expr = eq_fnode->mutable_exprs()->Add();
  auto *rhs_eq_expr = eq_fnode->mutable_exprs()->Add();

  auto *ceil_fnode = lhs_eq_expr->mutable_function_node();
  ceil_fnode->set_function_op(proto_msg::FunctionOP::NUM_CEILING);
  auto *lhs_var_expr = ceil_fnode->mutable_exprs()->Add();
  auto *lhs_var_term = lhs_var_expr->mutable_term_node();
  lhs_var_term->set_term_value("?x");
  lhs_var_term->set_term_type(proto_msg::TermType::VARIABLE);

  auto *mult_fnode = rhs_eq_expr->mutable_function_node();
  mult_fnode->set_function_op(proto_msg::FunctionOP::MULTIPLY);
  auto *lhs_div_expr = mult_fnode->mutable_exprs()->Add();
  auto *rhs_div_expr = mult_fnode->mutable_exprs()->Add();
  auto *floor_fnode = lhs_div_expr->mutable_function_node();
  floor_fnode->set_function_op(proto_msg::FunctionOP::NUM_FLOOR);

  auto *div_fnode =
      floor_fnode->mutable_exprs()->Add()->mutable_function_node();
  div_fnode->set_function_op(proto_msg::FunctionOP::DIVIDE);

  rhs_div_expr->mutable_term_node()->set_term_value("\"2\"^^xsd:integer");
  rhs_div_expr->mutable_term_node()->set_term_type(
      proto_msg::TermType::LITERAL);

  auto *div_lhs_expr = div_fnode->mutable_exprs()->Add();
  auto *div_rhs_expr = div_fnode->mutable_exprs()->Add();

  auto *abs_fnode = div_lhs_expr->mutable_function_node();
  abs_fnode->set_function_op(proto_msg::FunctionOP::NUM_CEILING);
  auto *var_expr = abs_fnode->mutable_exprs()->Add();
  auto *var_term = var_expr->mutable_term_node();
  var_term->set_term_value("?x");
  var_term->set_term_type(proto_msg::TermType::VARIABLE);

  div_rhs_expr->mutable_term_node()->set_term_type(
      proto_msg::TermType::LITERAL);
  div_rhs_expr->mutable_term_node()->set_term_value("\"2\"^^xsd:double");

  auto result = QueryFiltersFixture::cache->run_query(tree);

  auto query_values_set = get_values_double_from_result_table(
      result.table(), *QueryFiltersFixture::pcm);

  std::set<double> expected_values;

  for (auto value : values_double1) {
    if ((int)std::ceil(value) % 2 == 0)
      expected_values.insert(value);
  }

  ASSERT_EQ(query_values_set, expected_values);
}

TEST_F(QueryFiltersFixture, test_num_round_floor_eval_1) {
  proto_msg::SparqlTree tree;
  auto *distinct_node = tree.mutable_root()->mutable_distinct_node();
  auto *project_node =
      distinct_node->mutable_sub_node()->mutable_project_node();
  project_node->add_vars("?x");
  auto *filter_node = project_node->mutable_sub_op()->mutable_filter_node();
  auto *bgp_node = filter_node->mutable_node()->mutable_bgp_node();
  auto *first_triple = bgp_node->mutable_triple()->Add();
  auto *first_subject = first_triple->mutable_subject();
  auto *first_predicate = first_triple->mutable_predicate();
  auto *first_object = first_triple->mutable_object();
  first_subject->set_term_value("?y");
  first_subject->set_term_type(proto_msg::TermType::VARIABLE);
  first_predicate->set_term_value("<has_double>");
  first_predicate->set_term_type(proto_msg::TermType::IRI);
  first_object->set_term_value("?x");
  first_object->set_term_type(proto_msg::TermType::VARIABLE);

  auto *eq_expr = filter_node->mutable_exprs()->Add();
  auto *eq_fnode = eq_expr->mutable_function_node();
  eq_fnode->set_function_op(proto_msg::FunctionOP::EQUALS);

  auto *lhs_eq_expr = eq_fnode->mutable_exprs()->Add();
  auto *rhs_eq_expr = eq_fnode->mutable_exprs()->Add();

  auto *ceil_fnode = lhs_eq_expr->mutable_function_node();
  ceil_fnode->set_function_op(proto_msg::FunctionOP::NUM_ROUND);
  auto *lhs_var_expr = ceil_fnode->mutable_exprs()->Add();
  auto *lhs_var_term = lhs_var_expr->mutable_term_node();
  lhs_var_term->set_term_value("?x");
  lhs_var_term->set_term_type(proto_msg::TermType::VARIABLE);

  auto *mult_fnode = rhs_eq_expr->mutable_function_node();
  mult_fnode->set_function_op(proto_msg::FunctionOP::MULTIPLY);
  auto *lhs_div_expr = mult_fnode->mutable_exprs()->Add();
  auto *rhs_div_expr = mult_fnode->mutable_exprs()->Add();
  auto *floor_fnode = lhs_div_expr->mutable_function_node();
  floor_fnode->set_function_op(proto_msg::FunctionOP::NUM_FLOOR);

  auto *div_fnode =
      floor_fnode->mutable_exprs()->Add()->mutable_function_node();
  div_fnode->set_function_op(proto_msg::FunctionOP::DIVIDE);

  rhs_div_expr->mutable_term_node()->set_term_value("\"2\"^^xsd:integer");
  rhs_div_expr->mutable_term_node()->set_term_type(
      proto_msg::TermType::LITERAL);

  auto *div_lhs_expr = div_fnode->mutable_exprs()->Add();
  auto *div_rhs_expr = div_fnode->mutable_exprs()->Add();

  auto *abs_fnode = div_lhs_expr->mutable_function_node();
  abs_fnode->set_function_op(proto_msg::FunctionOP::NUM_ROUND);
  auto *var_expr = abs_fnode->mutable_exprs()->Add();
  auto *var_term = var_expr->mutable_term_node();
  var_term->set_term_value("?x");
  var_term->set_term_type(proto_msg::TermType::VARIABLE);

  div_rhs_expr->mutable_term_node()->set_term_type(
      proto_msg::TermType::LITERAL);
  div_rhs_expr->mutable_term_node()->set_term_value("\"2\"^^xsd:double");

  auto result = QueryFiltersFixture::cache->run_query(tree);

  auto query_values_set = get_values_double_from_result_table(
      result.table(), *QueryFiltersFixture::pcm);

  std::set<double> expected_values;

  for (auto value : values_double1) {
    if ((int)std::round(value) % 2 == 0)
      expected_values.insert(value);
  }

  ASSERT_EQ(query_values_set, expected_values);
}

TEST_F(QueryFiltersFixture, test_regex_eval_1) {
  proto_msg::SparqlTree tree;
  auto *distinct_node = tree.mutable_root()->mutable_distinct_node();
  auto *project_node =
      distinct_node->mutable_sub_node()->mutable_project_node();
  project_node->add_vars("?x");
  auto *filter_node = project_node->mutable_sub_op()->mutable_filter_node();
  auto *bgp_node = filter_node->mutable_node()->mutable_bgp_node();
  auto *first_triple = bgp_node->mutable_triple()->Add();
  auto *first_subject = first_triple->mutable_subject();
  auto *first_predicate = first_triple->mutable_predicate();
  auto *first_object = first_triple->mutable_object();
  first_subject->set_term_value("?y");
  first_subject->set_term_type(proto_msg::TermType::VARIABLE);
  first_predicate->set_term_value("<has_date>");
  first_predicate->set_term_type(proto_msg::TermType::IRI);
  first_object->set_term_value("?x");
  first_object->set_term_type(proto_msg::TermType::VARIABLE);

  auto *regex_fnode =
      filter_node->mutable_exprs()->Add()->mutable_function_node();
  regex_fnode->set_function_op(proto_msg::FunctionOP::REGEX);

  auto *text_term = regex_fnode->mutable_exprs()->Add()->mutable_term_node();
  text_term->set_term_value("?x");
  text_term->set_term_type(proto_msg::TermType::VARIABLE);

  auto *pattern_term = regex_fnode->mutable_exprs()->Add()->mutable_term_node();
  pattern_term->set_term_value("\"^2025-.*\"");
  pattern_term->set_term_type(proto_msg::TermType::LITERAL);

  auto *flags_term = regex_fnode->mutable_exprs()->Add()->mutable_term_node();
  flags_term->set_term_value("\"i\"");
  flags_term->set_term_type(proto_msg::TermType::LITERAL);

  auto result = QueryFiltersFixture::cache->run_query(tree);

  print_table_debug2(result, *cache);

  auto query_values_set = get_string_values_from_result_table(
      result.table(), *QueryFiltersFixture::pcm);

  std::set<std::string> query_values_set_content;

  for (const auto &single_result : query_values_set) {
    auto literal_data =
        StringHandlingUtil::extract_literal_data_from_string(single_result);
    query_values_set_content.insert(std::move(literal_data.value));
  }

  std::set<std::string> expected_values;
  for (const auto &date : dates) {
    if (StringHandlingUtil::starts_with(date, "2025-"))
      expected_values.insert(date);
  }
  ASSERT_EQ(query_values_set_content, expected_values);
}

static void run_test_same_term_one(const std::string &predicate_str,
                                   const std::string &term_str,
                                   QueryResult &out_result,
                                   std::set<std::string> &out_string_set) {
  proto_msg::SparqlTree tree;
  auto *distinct_node = tree.mutable_root()->mutable_distinct_node();
  auto *project_node =
      distinct_node->mutable_sub_node()->mutable_project_node();
  project_node->add_vars("?x");
  auto *filter_node = project_node->mutable_sub_op()->mutable_filter_node();
  auto *bgp_node = filter_node->mutable_node()->mutable_bgp_node();
  auto *first_triple = bgp_node->mutable_triple()->Add();
  auto *first_subject = first_triple->mutable_subject();
  auto *first_predicate = first_triple->mutable_predicate();
  auto *first_object = first_triple->mutable_object();
  first_subject->set_term_value("?y");
  first_subject->set_term_type(proto_msg::TermType::VARIABLE);
  first_predicate->set_term_value(predicate_str);
  first_predicate->set_term_type(proto_msg::TermType::IRI);
  first_object->set_term_value("?x");
  first_object->set_term_type(proto_msg::TermType::VARIABLE);

  auto *same_term_fnode =
      filter_node->mutable_exprs()->Add()->mutable_function_node();
  same_term_fnode->set_function_op(proto_msg::FunctionOP::SAME_TERM);

  auto *lhs_term_node =
      same_term_fnode->mutable_exprs()->Add()->mutable_term_node();
  auto *rhs_term_node =
      same_term_fnode->mutable_exprs()->Add()->mutable_term_node();

  lhs_term_node->set_term_type(proto_msg::TermType::VARIABLE);
  lhs_term_node->set_term_value("?x");

  rhs_term_node->set_term_type(proto_msg::TermType::LITERAL);
  rhs_term_node->set_term_value(term_str);

  auto result = QueryFiltersFixture::cache->run_query(tree);

  // print_table_debug2(result, *QueryFiltersFixture::cache);

  auto query_values_set = get_string_values_from_result_table(
      result.table(), *QueryFiltersFixture::pcm);

  ASSERT_EQ(query_values_set.size(), 1);
  ASSERT_NE(query_values_set.find(term_str), query_values_set.end());

  out_result = std::move(result);
  out_string_set = std::move(query_values_set);
}

TEST_F(QueryFiltersFixture, test_same_term_eval_dates_1) {
  QueryResult query_result;
  std::set<std::string> results_str_set;
  for (const auto &date : dates) {
    run_test_same_term_one("<has_date>", "\"" + date + "\"^^xsd:dateTime",
                           query_result, results_str_set);
  }
}

TEST_F(QueryFiltersFixture, test_same_term_eval_integers_1) {
  QueryResult query_result;
  std::set<std::string> results_str_set;
  for (const auto &value : values1) {
    run_test_same_term_one("<has_integer>",
                           "\"" + std::to_string(value) + "\"^^xsd:integer",
                           query_result, results_str_set);
  }
}

TEST_F(QueryFiltersFixture, test_same_term_eval_doubles_1) {
  QueryResult query_result;
  std::set<std::string> results_str_set;
  for (const auto &value : values_double1) {
    run_test_same_term_one("<has_double>",
                           "\"" + std::to_string(value) + "\"^^xsd:double",
                           query_result, results_str_set);
  }
}

TEST_F(QueryFiltersFixture, test_str_after_1) {
  proto_msg::SparqlTree tree;
  auto *distinct_node = tree.mutable_root()->mutable_distinct_node();
  auto *project_node =
      distinct_node->mutable_sub_node()->mutable_project_node();
  project_node->add_vars("?x");
  auto *filter_node = project_node->mutable_sub_op()->mutable_filter_node();
  auto *bgp_node = filter_node->mutable_node()->mutable_bgp_node();
  auto *first_triple = bgp_node->mutable_triple()->Add();
  auto *first_subject = first_triple->mutable_subject();
  auto *first_predicate = first_triple->mutable_predicate();
  auto *first_object = first_triple->mutable_object();
  first_subject->set_term_value("?y");
  first_subject->set_term_type(proto_msg::TermType::VARIABLE);
  first_predicate->set_term_value("<has_date>");
  first_predicate->set_term_type(proto_msg::TermType::IRI);
  first_object->set_term_value("?x");
  first_object->set_term_type(proto_msg::TermType::VARIABLE);

  auto *equals_fnode =
      filter_node->mutable_exprs()->Add()->mutable_function_node();
  equals_fnode->set_function_op(proto_msg::FunctionOP::EQUALS);

  auto *str_after_fnode =
      equals_fnode->mutable_exprs()->Add()->mutable_function_node();
  auto *suffix_query_term =
      equals_fnode->mutable_exprs()->Add()->mutable_term_node();

  str_after_fnode->set_function_op(proto_msg::FunctionOP::STR_AFTER);
  auto *text_term =
      str_after_fnode->mutable_exprs()->Add()->mutable_term_node();
  auto *pattern_term =
      str_after_fnode->mutable_exprs()->Add()->mutable_term_node();

  text_term->set_term_type(proto_msg::TermType::VARIABLE);
  text_term->set_term_value("?x");

  pattern_term->set_term_type(proto_msg::TermType::LITERAL);
  pattern_term->set_term_value("\"19:50:44\"");

  suffix_query_term->set_term_type(proto_msg::TermType::LITERAL);
  suffix_query_term->set_term_value("\".988Z\"");

  auto result = QueryFiltersFixture::cache->run_query(tree);

  auto query_values_set = get_string_values_from_result_table(
      result.table(), *QueryFiltersFixture::pcm);

  std::set<std::string> expected_dates;

  for (const auto &date : dates) {
    if (StringHandlingUtil::ends_with(date, "19:50:44.988Z")) {
      expected_dates.insert("\"" + date + "\"^^xsd:dateTime");
    }
  }

  ASSERT_EQ(query_values_set, expected_dates);
}

TEST_F(QueryFiltersFixture, test_str_before_1) {
  proto_msg::SparqlTree tree;
  auto *distinct_node = tree.mutable_root()->mutable_distinct_node();
  auto *project_node =
      distinct_node->mutable_sub_node()->mutable_project_node();
  project_node->add_vars("?x");
  auto *filter_node = project_node->mutable_sub_op()->mutable_filter_node();
  auto *bgp_node = filter_node->mutable_node()->mutable_bgp_node();
  auto *first_triple = bgp_node->mutable_triple()->Add();
  auto *first_subject = first_triple->mutable_subject();
  auto *first_predicate = first_triple->mutable_predicate();
  auto *first_object = first_triple->mutable_object();
  first_subject->set_term_value("?y");
  first_subject->set_term_type(proto_msg::TermType::VARIABLE);
  first_predicate->set_term_value("<has_date>");
  first_predicate->set_term_type(proto_msg::TermType::IRI);
  first_object->set_term_value("?x");
  first_object->set_term_type(proto_msg::TermType::VARIABLE);

  auto *equals_fnode =
      filter_node->mutable_exprs()->Add()->mutable_function_node();
  equals_fnode->set_function_op(proto_msg::FunctionOP::STR_STARTS_WITH);

  auto *str_after_fnode =
      equals_fnode->mutable_exprs()->Add()->mutable_function_node();
  auto *suffix_query_term =
      equals_fnode->mutable_exprs()->Add()->mutable_term_node();

  str_after_fnode->set_function_op(proto_msg::FunctionOP::STR_BEFORE);
  auto *text_term =
      str_after_fnode->mutable_exprs()->Add()->mutable_term_node();
  auto *pattern_term =
      str_after_fnode->mutable_exprs()->Add()->mutable_term_node();

  text_term->set_term_type(proto_msg::TermType::VARIABLE);
  text_term->set_term_value("?x");

  pattern_term->set_term_type(proto_msg::TermType::LITERAL);
  pattern_term->set_term_value("\"44.988Z\"");

  suffix_query_term->set_term_type(proto_msg::TermType::LITERAL);
  suffix_query_term->set_term_value("\"20\"");

  auto result = QueryFiltersFixture::cache->run_query(tree);

  auto query_values_set = get_string_values_from_result_table(
      result.table(), *QueryFiltersFixture::pcm);

  std::set<std::string> expected_dates;

  for (const auto &date : dates) {
    if (StringHandlingUtil::ends_with(date, "44.988Z") &&
        StringHandlingUtil::starts_with(date, "20")) {
      expected_dates.insert("\"" + date + "\"^^xsd:dateTime");
    }
  }

  ASSERT_EQ(query_values_set, expected_dates);
}
