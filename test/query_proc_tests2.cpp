//
// Created by cristobal on 5/17/21.
//

#include <chrono>
#include <cmath>
#include <filesystem>
#include <gtest/gtest.h>
#include <memory>

#include "cache_test_util.hpp"

#include <Cache.hpp>
#include <EmptyISDManager.hpp>
#include <PredicatesCacheManager.hpp>
#include <hashing.hpp>
#include <query_processing/utility/StringHandlingUtil.hpp>
#include <query_processing/utility/UuidGenerator.hpp>

using namespace std::chrono_literals;
TimeControl time_control(1e12, 100min);

namespace fs = std::filesystem;
class QueryProcTests2Fixture : public ::testing::Test {
protected:
  virtual void SetUp() override {}
  virtual void TearDown() override {}
  virtual void TestBody() override {}

public:
  ~QueryProcTests2Fixture() override {
    google::protobuf::ShutdownProtobufLibrary();
  }

  static std::string fname;
  static std::shared_ptr<PredicatesCacheManager> pcm;
  static std::unique_ptr<Cache> cache;
  static std::set<int> values1;
  static std::set<int> values2;
  static std::set<double> values_double1;
  static std::vector<std::string> dates;
  static std::vector<std::string> some_literal_strings1;
  static std::vector<std::string> some_intermediate_iris1;
  static std::vector<std::string> some_intermediate_iris2;
  static std::vector<DateInfo> date_infos_expected;

  static std::string p21;
  static std::string p214;
  static std::string p2639;
  static std::string p27;
  static std::string p31;
  static std::string p570;
  static std::string p629;
  static std::string p650;
  static std::string q4789e;
  static std::string q5;
  static std::string q658e;
  static std::string q1036;
  static std::string s_about;
  static std::string s_inlang;
  static std::string s_partof;
  static std::string es_wiki_org;
  static std::string fr_ltag;
  static std::string ns_type;
  static std::string ont_prop;
  static size_t iterations_count_1;
  static int problematic_query_2_input_size;

  static int pq3_sz;
  static int pq4_sz;
  static int pq5_sz;

  static std::string some_iri_gen(size_t id) {
    return "<some_iri_" + std::to_string(id) + ">";
  };
  static std::string some_other_iri_gen(size_t id) {
    return "<some_other_iri_" + std::to_string(id) + ">";
  };

  static void SetUpTestCase() {
    fname = "predicates.bin";
    build_cache_test_file(fname);
    pcm = std::make_shared<PredicatesCacheManager>(
        std::make_unique<EmptyISDManager>(), fname);
    values1 = {-100, 42, 1, 2, 3, -1, 4, 400, 98, 99, 49, 50, -30};
    values2 = {400, 98, 99, 49, 50, -30};
    values_double1 = {-100.54, 42.33,    1.123,   2.65,     3.14,    -1.024,
                      4.99,    400.8974, 98.0023, 99.00005, 49.0123, 50.99991};

    for (size_t i = 0; i < values1.size(); i++) {
      some_intermediate_iris1.push_back("<intermediate_ref_" +
                                        std::to_string(i) + ">");
      some_intermediate_iris2.push_back("<intermediate_ref2_" +
                                        std::to_string(i) + ">");
    }

    for (size_t i = 0; i < some_intermediate_iris1.size(); i++) {
      auto &iri1 = some_intermediate_iris1[i];
      auto &iri2 = some_intermediate_iris2[i];
      pcm->add_triple(RDFTripleResource(RDFResource(iri1, RDF_TYPE_IRI),
                                        RDFResource("<has_ref>", RDF_TYPE_IRI),
                                        RDFResource(iri2, RDF_TYPE_IRI)));
    }

    {
      size_t v1_i = 0;
      for (auto value : values1) {
        auto &iri2 = some_intermediate_iris2[v1_i];
        pcm->add_triple(RDFTripleResource(
            RDFResource(iri2, RDF_TYPE_IRI),
            RDFResource("<has_integer>", RDF_TYPE_IRI),
            RDFResource("\"" + std::to_string(value) + "\"^^xsd:integer",
                        RDFResourceType::RDF_TYPE_LITERAL)));
        v1_i++;
      }
    }

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
      date_infos_expected.push_back(ParsingUtils::parse_iso8601(date));
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
    CacheArgs args;
    args.update_log_filename = "ulf.bin";
    args.replacement_strategy = I_CacheReplacement::REPLACEMENT_STRATEGY::NO_CACHING;
    args.memory_budget_bytes = 100'000;
    args.time_out_ms = 5000;
    args.temp_files_dir = "./";
    cache = std::make_unique<Cache>(
        pcm, args);
  }

  static void add_problematic_query_1_input() {
    for (size_t i = 0; i < iterations_count_1; i++) {
      auto iri_value_var1 = some_iri_gen(i);
      auto iri_value_var2 = some_other_iri_gen(i);
      pcm->add_triple(RDFTripleResource(
          RDFResource(iri_value_var1, RDFResourceType::RDF_TYPE_IRI),
          RDFResource(p21, RDFResourceType::RDF_TYPE_IRI),
          RDFResource(q658e, RDFResourceType::RDF_TYPE_IRI)));
      pcm->add_triple(RDFTripleResource(
          RDFResource(iri_value_var1, RDFResourceType::RDF_TYPE_IRI),
          RDFResource(p27, RDFResourceType::RDF_TYPE_IRI),
          RDFResource(q1036, RDFResourceType::RDF_TYPE_IRI)));
      pcm->add_triple(RDFTripleResource(
          RDFResource(iri_value_var2, RDFResourceType::RDF_TYPE_IRI),
          RDFResource(s_about, RDFResourceType::RDF_TYPE_IRI),
          RDFResource(iri_value_var1, RDFResourceType::RDF_TYPE_IRI)));
    }
  }

  static void TearDownTestCase() {
    fs::remove(fname);
    pcm = nullptr;
    cache = nullptr;
  }

  static proto_msg::SparqlTree generate_problematic_query_1() {
    proto_msg::SparqlTree tree;

    auto *slice_node = tree.mutable_root()->mutable_slice_node();
    slice_node->set_start(std::numeric_limits<long>::min());
    slice_node->set_length(1000);
    auto *project_node = slice_node->mutable_node()->mutable_project_node();
    project_node->add_vars("?var1");
    auto *filter_node = project_node->mutable_sub_op()->mutable_filter_node();
    // auto *sequence_node =
    // project_node->mutable_sub_op()->mutable_sequence_node();

    auto *bgp_filter_node = filter_node->mutable_node()->mutable_bgp_node();

    auto *first_triple = bgp_filter_node->mutable_triple()->Add();
    auto *first_triple_subject = first_triple->mutable_subject();
    auto *first_triple_predicate = first_triple->mutable_predicate();
    auto *first_triple_object = first_triple->mutable_object();
    first_triple_subject->set_term_type(proto_msg::TermType::VARIABLE);
    first_triple_subject->set_term_value("?var1");
    first_triple_predicate->set_term_type(proto_msg::TermType::IRI);
    first_triple_predicate->set_term_value(p21);
    first_triple_object->set_term_type(proto_msg::TermType::IRI);
    first_triple_object->set_term_value(q658e);

    auto *second_triple = bgp_filter_node->mutable_triple()->Add();
    auto *second_triple_subject = second_triple->mutable_subject();
    auto *second_triple_predicate = second_triple->mutable_predicate();
    auto *second_triple_object = second_triple->mutable_object();
    second_triple_subject->set_term_type(proto_msg::TermType::VARIABLE);
    second_triple_subject->set_term_value("?var1");
    second_triple_predicate->set_term_type(proto_msg::TermType::IRI);
    second_triple_predicate->set_term_value(p27);
    second_triple_object->set_term_type(proto_msg::TermType::IRI);
    second_triple_object->set_term_value(q1036);

    auto *third_triple = bgp_filter_node->mutable_triple()->Add();
    auto *third_triple_subject = third_triple->mutable_subject();
    auto *third_triple_predicate = third_triple->mutable_predicate();
    auto *third_triple_object = third_triple->mutable_object();
    third_triple_subject->set_term_type(proto_msg::TermType::VARIABLE);
    third_triple_subject->set_term_value("?var2");
    third_triple_predicate->set_term_type(proto_msg::TermType::IRI);
    third_triple_predicate->set_term_value(s_about);
    third_triple_object->set_term_type(proto_msg::TermType::VARIABLE);
    third_triple_object->set_term_value("?var1");

    auto *fnode = filter_node->mutable_exprs()->Add()->mutable_function_node();
    fnode->set_function_op(proto_msg::FunctionOP::NOT_EXISTS);
    auto *pattern_bgp = fnode->mutable_exprs()
                            ->Add()
                            ->mutable_pattern_node()
                            ->mutable_bgp_node();

    auto *first_triple_pattern = pattern_bgp->mutable_triple()->Add();
    auto *first_triple_pattern_subject =
        first_triple_pattern->mutable_subject();
    auto *first_triple_pattern_predicate =
        first_triple_pattern->mutable_predicate();
    auto *first_triple_pattern_object = first_triple_pattern->mutable_object();
    first_triple_pattern_subject->set_term_type(proto_msg::TermType::VARIABLE);
    first_triple_pattern_subject->set_term_value("?var3");
    first_triple_pattern_predicate->set_term_type(proto_msg::TermType::IRI);
    first_triple_pattern_predicate->set_term_value(s_about);
    first_triple_pattern_object->set_term_type(proto_msg::TermType::VARIABLE);
    first_triple_pattern_object->set_term_value("?var1");

    auto *second_triple_pattern = pattern_bgp->mutable_triple()->Add();
    auto *second_triple_pattern_subject =
        second_triple_pattern->mutable_subject();
    auto *second_triple_pattern_predicate =
        second_triple_pattern->mutable_predicate();
    auto *second_triple_pattern_object =
        second_triple_pattern->mutable_object();
    second_triple_pattern_subject->set_term_type(proto_msg::TermType::VARIABLE);
    second_triple_pattern_subject->set_term_value("?var3");
    second_triple_pattern_predicate->set_term_type(proto_msg::TermType::IRI);
    second_triple_pattern_predicate->set_term_value(s_inlang);
    second_triple_pattern_object->set_term_type(proto_msg::TermType::LITERAL);
    second_triple_pattern_object->set_term_value(fr_ltag);
    return tree;
  }

  static std::string generate_pq2_iri1(int i) {
    return "<some_iri_1_fq2_" + std::to_string(i) + ">";
  }

  static std::string generate_pq2_iri2(int i) {
    return "<some_iri_2_fq2_" + std::to_string(i) + ">";
  }
  static std::string generate_pq2_iri3(int i) {
    return "<some_iri_3_fq2_" + std::to_string(i) + ">";
  }

  static void add_problematic_query_2_input() {
    for (int i = 0; i < problematic_query_2_input_size; i++) {
      auto var_1_value = generate_pq2_iri1(i);
      auto var_2_value = generate_pq2_iri2(i);
      pcm->add_triple(RDFTripleResource(RDFResource(var_1_value, RDF_TYPE_IRI),
                                        RDFResource(p21, RDF_TYPE_IRI),
                                        RDFResource(q658e, RDF_TYPE_IRI)));
      pcm->add_triple(
          RDFTripleResource(RDFResource(var_1_value, RDF_TYPE_IRI),
                            RDFResource(p650, RDF_TYPE_IRI),
                            RDFResource(var_2_value, RDF_TYPE_IRI)));

      pcm->add_triple(RDFTripleResource(RDFResource(var_1_value, RDF_TYPE_IRI),
                                        RDFResource(p31, RDF_TYPE_IRI),
                                        RDFResource(q5, RDF_TYPE_IRI)));
    }
  }

  static void add_problematic_query_2_cond_input(int total) {
    for (int i = 0;
         i < std::max(0, std::min(problematic_query_2_input_size, total));
         i++) {
      auto var_1_value = generate_pq2_iri1(i);
      auto var_3_value = generate_pq2_iri3(i);
      pcm->add_triple(
          RDFTripleResource(RDFResource(var_3_value, RDF_TYPE_IRI),
                            RDFResource(s_about, RDF_TYPE_IRI),
                            RDFResource(var_1_value, RDF_TYPE_IRI)));
      pcm->add_triple(
          RDFTripleResource(RDFResource(var_3_value, RDF_TYPE_IRI),
                            RDFResource(s_inlang, RDF_TYPE_IRI),
                            RDFResource("\"nl\"", RDF_TYPE_LITERAL)));
    }
  }

  static proto_msg::SparqlTree generate_problematic_query_2() {
    proto_msg::SparqlTree tree;
    auto *slice_node = tree.mutable_root()->mutable_slice_node();
    slice_node->set_length(1000);
    slice_node->set_start(std::numeric_limits<long>::min());
    auto *project_node = slice_node->mutable_node()->mutable_project_node();
    project_node->add_vars("?var1");
    auto *filter_node = project_node->mutable_sub_op()->mutable_filter_node();
    auto *filter_fnode =
        filter_node->mutable_exprs()->Add()->mutable_function_node();
    filter_fnode->set_function_op(proto_msg::LOGICAL_NOT);
    auto *bound_expr =
        filter_fnode->mutable_exprs()->Add()->mutable_function_node();
    bound_expr->set_function_op(proto_msg::BOUND);
    auto *bound_term = bound_expr->mutable_exprs()->Add()->mutable_term_node();
    bound_term->set_term_value("?var3");
    bound_term->set_term_type(proto_msg::VARIABLE);
    bound_term->set_basic_type(proto_msg::STRING);
    auto *sequence_node = filter_node->mutable_node()->mutable_sequence_node();

    auto *optional_node =
        sequence_node->mutable_nodes()->Add()->mutable_optional_node();
    auto *left_node = optional_node->mutable_left_node()->mutable_bgp_node();
    auto *right_node = optional_node->mutable_right_node()->mutable_bgp_node();
    auto *left_first_triple = left_node->mutable_triple()->Add();
    auto *left_second_triple = left_node->mutable_triple()->Add();

    auto *left_first_triple_s = left_first_triple->mutable_subject();
    auto *left_first_triple_p = left_first_triple->mutable_predicate();
    auto *left_first_triple_o = left_first_triple->mutable_object();
    left_first_triple_s->set_term_type(proto_msg::VARIABLE);
    left_first_triple_s->set_term_value("?var1");
    left_first_triple_p->set_term_type(proto_msg::IRI);
    left_first_triple_p->set_term_value(p21);
    left_first_triple_o->set_term_type(proto_msg::IRI);
    left_first_triple_o->set_term_value(q658e);

    auto *left_second_triple_s = left_second_triple->mutable_subject();
    auto *left_second_triple_p = left_second_triple->mutable_predicate();
    auto *left_second_triple_o = left_second_triple->mutable_object();
    left_second_triple_s->set_term_type(proto_msg::VARIABLE);
    left_second_triple_s->set_term_value("?var1");
    left_second_triple_p->set_term_type(proto_msg::IRI);
    left_second_triple_p->set_term_value(p650);
    left_second_triple_o->set_term_type(proto_msg::VARIABLE);
    left_second_triple_o->set_term_value("?var2");

    auto *right_first_triple = right_node->mutable_triple()->Add();
    auto *right_second_triple = right_node->mutable_triple()->Add();

    auto *right_first_triple_s = right_first_triple->mutable_subject();
    auto *right_first_triple_p = right_first_triple->mutable_predicate();
    auto *right_first_triple_o = right_first_triple->mutable_object();
    right_first_triple_s->set_term_type(proto_msg::VARIABLE);
    right_first_triple_s->set_term_value("?var3");
    right_first_triple_p->set_term_type(proto_msg::IRI);
    right_first_triple_p->set_term_value(s_about);
    right_first_triple_o->set_term_type(proto_msg::VARIABLE);
    right_first_triple_o->set_term_value("?var1");

    auto *right_second_triple_s = right_second_triple->mutable_subject();
    auto *right_second_triple_p = right_second_triple->mutable_predicate();
    auto *right_second_triple_o = right_second_triple->mutable_object();
    right_second_triple_s->set_term_type(proto_msg::VARIABLE);
    right_second_triple_s->set_term_value("?var3");
    right_second_triple_p->set_term_type(proto_msg::IRI);
    right_second_triple_p->set_term_value(s_inlang);
    right_second_triple_o->set_term_type(proto_msg::LITERAL);
    right_second_triple_o->set_term_value("\"nl\"");

    auto *second_seq_bgp =
        sequence_node->mutable_nodes()->Add()->mutable_bgp_node();
    auto *triple_second_seq = second_seq_bgp->mutable_triple()->Add();
    auto *triple_second_seq_s = triple_second_seq->mutable_subject();
    auto *triple_second_seq_p = triple_second_seq->mutable_predicate();
    auto *triple_second_seq_o = triple_second_seq->mutable_object();
    triple_second_seq_s->set_term_type(proto_msg::VARIABLE);
    triple_second_seq_s->set_term_value("?var1");
    triple_second_seq_p->set_term_type(proto_msg::IRI);
    triple_second_seq_p->set_term_value(p31);
    triple_second_seq_o->set_term_type(proto_msg::IRI);
    triple_second_seq_o->set_term_value(q5);

    return tree;
  }

  static std::string pq3_var_1_gen(int i) {
    return "<pq3_iri_var1_" + std::to_string(i) + ">";
  }
  static void add_problematic_query_3_input() {
    for (int i = 0; i < pq3_sz; i++) {
      auto var_1_value = pq3_var_1_gen(i);
      pcm->add_triple(RDFTripleResource(RDFResource(var_1_value, RDF_TYPE_IRI),
                                        RDFResource(p629, RDF_TYPE_IRI),
                                        RDFResource(q4789e, RDF_TYPE_IRI)));
    }
  }
  static void add_problematic_query_3_input_not_exist(int until) {
    for (int i = 0; i < until; i++) {
      auto var_1_value = pq3_var_1_gen(i);

      pcm->add_triple(RDFTripleResource(RDFResource(var_1_value, RDF_TYPE_IRI),
                                        RDFResource(ns_type, RDF_TYPE_IRI),
                                        RDFResource(ont_prop, RDF_TYPE_IRI)));
    }
  }

  static void add_problematic_query_4_input() {
    auto numgen = [](int i) { return std::to_string(i % 2 == 0 ? 302 : 307); };
    for (int i = 0; i < pq4_sz; i++) {
      auto var1_val = "<pq4_iri_val_var_1_" + std::to_string(i) + ">";
      // auto var2_val = "<pq4_iri_val_var_2_" + std::to_string(i) + ">";
      auto var2_val = "\"" + numgen(i) + "\"^^xsd:integer";
      // auto var3_val = "<pq4_iri_val_var_3_" + std::to_string(i) + ">";
      auto var4_val = "<pq4_iri_val_var_4_" + std::to_string(i) + ">";

      pcm->add_triple(RDFTripleResource(
          RDFResource(var1_val, RDF_TYPE_IRI), RDFResource(p570, RDF_TYPE_IRI),
          RDFResource(var2_val, RDF_TYPE_LITERAL)));
      pcm->add_triple(RDFTripleResource(RDFResource(var4_val, RDF_TYPE_IRI),
                                        RDFResource(s_about, RDF_TYPE_IRI),
                                        RDFResource(var1_val, RDF_TYPE_IRI)));
      pcm->add_triple(
          RDFTripleResource(RDFResource(var4_val, RDF_TYPE_IRI),
                            RDFResource(s_inlang, RDF_TYPE_IRI),
                            RDFResource("\"es\"", RDF_TYPE_LITERAL)));
      pcm->add_triple(
          RDFTripleResource(RDFResource(var4_val, RDF_TYPE_IRI),
                            RDFResource(s_partof, RDF_TYPE_IRI),
                            RDFResource(es_wiki_org, RDF_TYPE_IRI)));
    }
  }

  static void add_problematic_query_5_input() {
    auto iri_gen = [](int v, int i) {
      return "<some_iri_pq5_var_" + std::to_string(v) + "_" +
             std::to_string(i) + ">";
    };

    static std::mt19937 generator(12345); // NOLINT(cert-msc51-cpp)
    static std::uniform_int_distribution random_distr(0, 1000000000);
    for (int i = 0; i < pq5_sz; i++) {
      auto var_1_val = iri_gen(1, i);
      auto var_2_val = UuidGenerator::generate_uuid_v4();
      auto var_3_val = std::to_string(random_distr(generator));
      pcm->add_triple(
          RDFTripleResource(RDFResource(var_1_val, RDF_TYPE_IRI),
                            RDFResource(p2639, RDF_TYPE_IRI),
                            RDFResource(var_2_val, RDF_TYPE_LITERAL)));
      pcm->add_triple(RDFTripleResource(
          RDFResource(var_1_val, RDF_TYPE_IRI), RDFResource(p214, RDF_TYPE_IRI),
          RDFResource(var_3_val, RDF_TYPE_LITERAL)));
    }
  }

  static proto_msg::SparqlTree generate_problematic_query_3() {
    proto_msg::SparqlTree tree;

    auto *slice_node = tree.mutable_root()->mutable_slice_node();
    slice_node->set_start(std::numeric_limits<long>::min());
    slice_node->set_length(1000);
    auto *project_node = slice_node->mutable_node()->mutable_project_node();
    project_node->mutable_vars()->Add("?var1");
    auto *filter_node = project_node->mutable_sub_op()->mutable_filter_node();
    auto *bgp_node_filter = filter_node->mutable_node()->mutable_bgp_node();
    auto *filter_triple = bgp_node_filter->mutable_triple()->Add();
    auto *filter_triple_s = filter_triple->mutable_subject();
    auto *filter_triple_p = filter_triple->mutable_predicate();
    auto *filter_triple_o = filter_triple->mutable_object();
    filter_triple_s->set_term_type(proto_msg::VARIABLE);
    filter_triple_s->set_term_value("?var1");
    filter_triple_p->set_term_type(proto_msg::IRI);
    filter_triple_p->set_term_value(p629);
    filter_triple_o->set_term_type(proto_msg::IRI);
    filter_triple_o->set_term_value(q4789e);

    auto *filter_fnode =
        filter_node->mutable_exprs()->Add()->mutable_function_node();
    filter_fnode->set_function_op(proto_msg::NOT_EXISTS);
    auto *pattern_triple = filter_fnode->mutable_exprs()
                               ->Add()
                               ->mutable_pattern_node()
                               ->mutable_bgp_node()
                               ->mutable_triple()
                               ->Add();

    auto *pattern_s = pattern_triple->mutable_subject();
    auto *pattern_p = pattern_triple->mutable_predicate();
    auto *pattern_o = pattern_triple->mutable_object();
    pattern_s->set_term_type(proto_msg::VARIABLE);
    pattern_s->set_term_value("?var1");
    pattern_p->set_term_type(proto_msg::IRI);
    pattern_p->set_term_value(ns_type);
    pattern_o->set_term_type(proto_msg::IRI);
    pattern_o->set_term_value(ont_prop);
    return tree;
  }

  static proto_msg::SparqlTree generate_problematic_query_4() {

    proto_msg::SparqlTree tree;

    auto *sequence_node = tree.mutable_root()->mutable_sequence_node();
    auto *filter_node =
        sequence_node->mutable_nodes()->Add()->mutable_filter_node();

    auto *extend_node = filter_node->mutable_node()->mutable_extend_node();

    auto *table_node = extend_node->mutable_node()->mutable_table_node();
    table_node->mutable_rows()->Add();

    auto *assignment = extend_node->mutable_assignments()->Add();
    auto *assignment_var = assignment->mutable_var();
    assignment_var->set_term_type(proto_msg::TermType::VARIABLE);
    assignment_var->set_term_value("?var3");
    auto *assignment_expr = assignment->mutable_expr()->mutable_function_node();
    assignment_expr->set_function_op(proto_msg::FunctionOP::SUBSTRACT);
    auto *lhs_substract =
        assignment_expr->mutable_exprs()->Add()->mutable_term_node();
    lhs_substract->set_term_value("\"305\"^^xsd:integer");
    lhs_substract->set_term_type(proto_msg::LITERAL);
    lhs_substract->set_basic_type(proto_msg::NUMBER);
    auto *rhs_substract =
        assignment_expr->mutable_exprs()->Add()->mutable_term_node();
    rhs_substract->set_term_type(proto_msg::TermType::VARIABLE);
    rhs_substract->set_term_value("?var2");

    auto *filter_first_gte =
        filter_node->mutable_exprs()->Add()->mutable_function_node();
    filter_first_gte->set_function_op(
        proto_msg::FunctionOP::GREATER_THAN_OR_EQUAL);
    auto *gte_first =
        filter_first_gte->mutable_exprs()->Add()->mutable_term_node();
    gte_first->set_term_type(proto_msg::TermType::VARIABLE);
    gte_first->set_term_value("?var3");
    auto *gte_second =
        filter_first_gte->mutable_exprs()->Add()->mutable_term_node();
    gte_second->set_term_type(proto_msg::TermType::LITERAL);
    gte_second->set_term_value("0");
    gte_second->set_basic_type(proto_msg::BasicType::NUMBER);

    auto *filter_second_lte =
        filter_node->mutable_exprs()->Add()->mutable_function_node();
    filter_second_lte->set_function_op(
        proto_msg::FunctionOP::LESS_THAN_OR_EQUAL);
    auto *lte_first =
        filter_second_lte->mutable_exprs()->Add()->mutable_term_node();
    lte_first->set_term_type(proto_msg::TermType::VARIABLE);
    lte_first->set_term_value("?var3");
    auto *lte_second =
        filter_second_lte->mutable_exprs()->Add()->mutable_term_node();
    lte_second->set_term_type(proto_msg::TermType::LITERAL);
    lte_second->set_term_value("5");
    lte_second->set_basic_type(proto_msg::BasicType::NUMBER);

    auto *bgp_node = sequence_node->mutable_nodes()->Add()->mutable_bgp_node();

    auto *first_triple = bgp_node->mutable_triple()->Add();
    auto *first_triple_s = first_triple->mutable_subject();
    auto *first_triple_p = first_triple->mutable_predicate();
    auto *first_triple_o = first_triple->mutable_object();

    first_triple_s->set_term_type(proto_msg::VARIABLE);
    first_triple_s->set_term_value("?var1");
    first_triple_p->set_term_type(proto_msg::IRI);
    first_triple_p->set_term_value(p570);
    first_triple_o->set_term_type(proto_msg::VARIABLE);
    first_triple_o->set_term_value("?var2");

    auto *second_triple = bgp_node->mutable_triple()->Add();
    auto *second_triple_s = second_triple->mutable_subject();
    auto *second_triple_p = second_triple->mutable_predicate();
    auto *second_triple_o = second_triple->mutable_object();

    second_triple_s->set_term_type(proto_msg::VARIABLE);
    second_triple_s->set_term_value("?var4");
    second_triple_p->set_term_type(proto_msg::IRI);
    second_triple_p->set_term_value(s_about);
    second_triple_o->set_term_type(proto_msg::VARIABLE);
    second_triple_o->set_term_value("?var1");

    auto *third_triple = bgp_node->mutable_triple()->Add();
    auto *third_triple_s = third_triple->mutable_subject();
    auto *third_triple_p = third_triple->mutable_predicate();
    auto *third_triple_o = third_triple->mutable_object();

    third_triple_s->set_term_type(proto_msg::VARIABLE);
    third_triple_s->set_term_value("?var4");
    third_triple_p->set_term_type(proto_msg::IRI);
    third_triple_p->set_term_value(s_inlang);
    third_triple_o->set_term_type(proto_msg::LITERAL);
    third_triple_o->set_term_value("\"es\"");

    auto *fourth_triple = bgp_node->mutable_triple()->Add();
    auto *fourth_triple_s = fourth_triple->mutable_subject();
    auto *fourth_triple_p = fourth_triple->mutable_predicate();
    auto *fourth_triple_o = fourth_triple->mutable_object();

    fourth_triple_s->set_term_type(proto_msg::VARIABLE);
    fourth_triple_s->set_term_value("?var4");
    fourth_triple_p->set_term_type(proto_msg::IRI);
    fourth_triple_p->set_term_value(s_partof);
    fourth_triple_o->set_term_type(proto_msg::IRI);
    fourth_triple_o->set_term_value(es_wiki_org);

    return tree;
  }

  static proto_msg::SparqlTree generate_problematic_query_5() {
    proto_msg::SparqlTree tree;

    auto *order_node = tree.mutable_root()->mutable_order_node();

    auto *sc1 = order_node->mutable_sort_conditions()->Add();
    sc1->set_direction(proto_msg::ASCENDING);
    auto *sc1_term = sc1->mutable_expr()->mutable_term_node();
    sc1_term->set_term_type(proto_msg::VARIABLE);
    sc1_term->set_term_value("?var3");

    auto *distinct_node = order_node->mutable_node()->mutable_distinct_node();
    auto *project = distinct_node->mutable_sub_node()->mutable_project_node();

    project->mutable_vars()->Add("?var1");
    project->mutable_vars()->Add("?var2");
    project->mutable_vars()->Add("?var3");

    auto *filter = project->mutable_sub_op()->mutable_filter_node();
    auto *seq = filter->mutable_node()->mutable_sequence_node();
    auto *filter2 = seq->mutable_nodes()->Add()->mutable_filter_node();
    auto *bgp_1 = filter2->mutable_node()->mutable_bgp_node();
    auto *triple_1 = bgp_1->mutable_triple()->Add();
    auto *subj_1 = triple_1->mutable_subject();
    auto *pred_1 = triple_1->mutable_predicate();
    auto *obj_1 = triple_1->mutable_object();
    subj_1->set_term_type(proto_msg::VARIABLE);
    subj_1->set_term_value("?var1");
    pred_1->set_term_type(proto_msg::IRI);
    pred_1->set_term_value(p2639);
    obj_1->set_term_type(proto_msg::VARIABLE);
    obj_1->set_term_value("?var2");
    auto *filter2_e1 = filter2->mutable_exprs()->Add()->mutable_function_node();
    filter2_e1->set_function_op(proto_msg::BOUND);
    auto *bound1 = filter2_e1->mutable_exprs()->Add()->mutable_term_node();
    bound1->set_term_value("?var2");
    bound1->set_term_type(proto_msg::VARIABLE);

    auto *bgp_2 = seq->mutable_nodes()->Add()->mutable_bgp_node();
    auto *triple2 = bgp_2->mutable_triple()->Add();
    auto *subj_2 = triple2->mutable_subject();
    auto *pred_2 = triple2->mutable_predicate();
    auto *obj_2 = triple2->mutable_object();
    subj_2->set_term_type(proto_msg::VARIABLE);
    subj_2->set_term_value("?var1");
    pred_2->set_term_type(proto_msg::IRI);
    pred_2->set_term_value(p214);
    obj_2->set_term_type(proto_msg::VARIABLE);
    obj_2->set_term_value("?var3");

    auto *filter1_e1 = filter->mutable_exprs()->Add()->mutable_function_node();
    filter1_e1->set_function_op(proto_msg::BOUND);
    auto *bound2 = filter1_e1->mutable_exprs()->Add()->mutable_term_node();
    bound2->set_term_type(proto_msg::VARIABLE);
    bound2->set_term_value("?var3");

    return tree;
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
std::vector<std::string> QueryProcTests2Fixture::some_intermediate_iris1;
std::vector<std::string> QueryProcTests2Fixture::some_intermediate_iris2;
std::vector<DateInfo> QueryProcTests2Fixture::date_infos_expected;

std::string QueryProcTests2Fixture::p21 =
    "<http://www.wikidata.org/prop/direct/P21>";
std::string QueryProcTests2Fixture::p214 =
    "<http://www.wikidata.org/prop/direct/P214>";
std::string QueryProcTests2Fixture::p2639 =
    "<http://www.wikidata.org/prop/direct/P2639>";
std::string QueryProcTests2Fixture::p27 =
    "<http://www.wikidata.org/prop/direct/P27>";
std::string QueryProcTests2Fixture::p31 =
    "<http://www.wikidata.org/prop/direct/P31>";
std::string QueryProcTests2Fixture::p570 =
    "<http://www.wikidata.org/prop/direct/P570>";
std::string QueryProcTests2Fixture::p629 =
    "<http://www.wikidata.org/prop/direct/P629>";
std::string QueryProcTests2Fixture::p650 =
    "<http://www.wikidata.org/prop/direct/P650>";
std::string QueryProcTests2Fixture::q4789e =
    "<http://www.wikidata.org/entity/Q4789e>";
std::string QueryProcTests2Fixture::q5 = "<http://www.wikidata.org/entity/Q5>";
std::string QueryProcTests2Fixture::q658e =
    "<http://www.wikidata.org/entity/Q6581072>";
std::string QueryProcTests2Fixture::q1036 =
    "<http://www.wikidata.org/entity/Q1036>";
std::string QueryProcTests2Fixture::s_about = "<http://schema.org/about>";
std::string QueryProcTests2Fixture::s_inlang = "<http://schema.org/inLanguage>";
std::string QueryProcTests2Fixture::s_partof = "<http://schema.org/isPartOf>";
std::string QueryProcTests2Fixture::es_wiki_org = "<https://es.wikipedia.org/>";
std::string QueryProcTests2Fixture::fr_ltag = "\"fr\"";
std::string QueryProcTests2Fixture::ns_type =
    "<http://www.w3.org/1999/02/22-rdf-syntax-ns#type>";
std::string QueryProcTests2Fixture::ont_prop =
    "<http://wikiba.se/ontology#Property>";

size_t QueryProcTests2Fixture::iterations_count_1 = 1500;
int QueryProcTests2Fixture::problematic_query_2_input_size = 2000;
int QueryProcTests2Fixture::pq3_sz = 1600;
int QueryProcTests2Fixture::pq4_sz = 1600;
int QueryProcTests2Fixture::pq5_sz = 1600;

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

  auto result = QueryProcTests2Fixture::cache->run_query(tree, time_control)
                    ->as_query_result_original();

  auto query_values_set = get_values_from_result_table(
      result->table(), *QueryProcTests2Fixture::pcm);

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

  auto result = QueryProcTests2Fixture::cache->run_query(tree, time_control)
                    ->as_query_result_original();

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
  for (const auto &row : result->table().data) {
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

  auto result = QueryProcTests2Fixture::cache->run_query(tree, time_control)
                    ->as_query_result_original();

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
  for (const auto &row : result->table().data) {
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

TEST_F(QueryProcTests2Fixture, can_do_order_op_test_1) {
  proto_msg::SparqlTree tree;
  auto *order_node = tree.mutable_root()->mutable_order_node();
  auto *sort_condition_1 = order_node->mutable_sort_conditions()->Add();
  auto *term_x = sort_condition_1->mutable_expr()->mutable_term_node();
  term_x->set_term_value("?x");
  term_x->set_term_type(proto_msg::TermType::VARIABLE);
  sort_condition_1->set_direction(proto_msg::SortDirection::ASCENDING);
  auto *sort_condition_2 = order_node->mutable_sort_conditions()->Add();
  auto *term_w = sort_condition_2->mutable_expr()->mutable_term_node();
  term_w->set_term_value("?w");
  term_w->set_term_type(proto_msg::TermType::VARIABLE);
  sort_condition_2->set_direction(proto_msg::SortDirection::ASCENDING);

  auto *distinct_node = order_node->mutable_node()->mutable_distinct_node();
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
  first_object->set_term_type(proto_msg::TermType::VARIABLE);
  first_object->set_term_value("?x");

  second_subject->set_term_value("?uu");
  second_subject->set_term_type(proto_msg::TermType::VARIABLE);
  second_predicate->set_term_value("<has_date>");
  second_predicate->set_term_type(proto_msg::TermType::IRI);
  second_object->set_term_value("?w");
  second_object->set_term_type(proto_msg::TermType::VARIABLE);

  auto result = QueryProcTests2Fixture::cache->run_query(tree, time_control)
                    ->as_query_result_original();

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
  std::vector<std::pair<std::string, std::string>> query_results_values;
  for (const auto &row : result->table().data) {
    auto first_resource = pcm->extract_resource(row[0]);
    auto second_resource = pcm->extract_resource(row[1]);
    query_results_values.push_back(
        {std::move(first_resource.value), std::move(second_resource.value)});
  }

  std::vector<std::pair<std::string, std::string>> combinations_ordered;
  combinations_ordered.reserve(all_combinations.size());
  for (const auto &p : all_combinations) {
    combinations_ordered.push_back(p);
  }

  std::sort(combinations_ordered.begin(), combinations_ordered.end(),
            [](const std::pair<std::string, std::string> &lhs,
               const std::pair<std::string, std::string> &rhs) {
              auto lhs_first_literal =
                  ParsingUtils::extract_literal_content_from_string(lhs.first);
              auto rhs_first_literal =
                  ParsingUtils::extract_literal_content_from_string(rhs.first);
              auto lhs_int = std::stoi(lhs_first_literal);
              auto rhs_int = std::stoi(rhs_first_literal);
              auto first_cmp = lhs_int - rhs_int;
              if (first_cmp != 0)
                return first_cmp < 0;
              auto lhs_literal =
                  ParsingUtils::extract_literal_content_from_string(lhs.second);
              auto rhs_literal =
                  ParsingUtils::extract_literal_content_from_string(rhs.second);
              auto dinfo_lhs = ParsingUtils::parse_iso8601(lhs_literal);
              auto dinfo_rhs = ParsingUtils::parse_iso8601(rhs_literal);
              return dinfo_lhs.cmp_to(dinfo_rhs) < 0;
            });

  ASSERT_EQ(combinations_ordered.size(), query_results_values.size());
  ASSERT_EQ(combinations_ordered, query_results_values);
}

TEST_F(QueryProcTests2Fixture, problematic_query_1_without_exist_values) {
  add_problematic_query_1_input();
  auto tree = generate_problematic_query_1();

  auto result = QueryProcTests2Fixture::cache->run_query(tree, time_control)
                    ->as_query_result_original();
  ASSERT_EQ(result->table().data.size(), 1000);
  ASSERT_EQ(result->table().headers.size(), 1);
  std::set<unsigned long> values;
  for (auto &row : result->table().data) {
    values.insert(row[0]);
  }

  std::set<std::string> expected_str_values;
  for (size_t i = 0; i < iterations_count_1; i++) {
    expected_str_values.insert(some_iri_gen(i));
  }

  for (auto value : values) {
    auto resource = pcm->extract_resource(value);
    ASSERT_NE(expected_str_values.find(resource.value),
              expected_str_values.end());
  }
}

TEST_F(QueryProcTests2Fixture, problematic_query_1_with_exist_values) {
  add_problematic_query_1_input();
  auto tree = generate_problematic_query_1();

  auto some_other2_iri_gen = [](size_t id) {
    return "<some_other2_iri_" + std::to_string(id) + ">";
  };

  for (size_t i = 0; i < iterations_count_1 / 2; i++) {
    auto iri_value_var1 = some_iri_gen(i);
    auto iri_value_var2 = some_other_iri_gen(i);
    auto iri_value_var3 = some_other2_iri_gen(i);
    pcm->add_triple(RDFTripleResource(
        RDFResource(iri_value_var3, RDFResourceType::RDF_TYPE_IRI),
        RDFResource(s_about, RDFResourceType::RDF_TYPE_IRI),
        RDFResource(iri_value_var1, RDFResourceType::RDF_TYPE_IRI)));
    pcm->add_triple(RDFTripleResource(
        RDFResource(iri_value_var3, RDFResourceType::RDF_TYPE_IRI),
        RDFResource(s_inlang, RDFResourceType::RDF_TYPE_IRI),
        RDFResource(fr_ltag, RDFResourceType::RDF_TYPE_LITERAL)));
  }

  auto result = QueryProcTests2Fixture::cache->run_query(tree, time_control)
                    ->as_query_result_original();
  ASSERT_EQ(result->table().data.size(), iterations_count_1 / 2);

  ASSERT_EQ(result->table().headers.size(), 1);
  std::set<unsigned long> values;
  for (auto &row : result->table().data) {
    values.insert(row[0]);
  }

  std::set<std::string> expected_str_values;
  std::set<std::string> not_expected_str_values;
  size_t i = 0;
  for (; i < iterations_count_1 / 2; i++) {
    not_expected_str_values.insert(some_iri_gen(i));
  }
  for (; i < iterations_count_1; i++) {
    expected_str_values.insert(some_iri_gen(i));
  }

  for (auto value : values) {
    auto resource = pcm->extract_resource(value);
    ASSERT_NE(expected_str_values.find(resource.value),
              expected_str_values.end());
    ASSERT_EQ(not_expected_str_values.find(resource.value),
              not_expected_str_values.end());
  }
}

TEST_F(QueryProcTests2Fixture, inner_join_non_bgp_1_test) {
  proto_msg::SparqlTree tree;

  auto sequence_node = tree.mutable_root()->mutable_sequence_node();
  auto bgp1 = sequence_node->mutable_nodes()->Add()->mutable_bgp_node();
  auto bgp2 = sequence_node->mutable_nodes()->Add()->mutable_bgp_node();

  auto bgp_1_triple = bgp1->mutable_triple()->Add();
  auto bgp_1_subject = bgp_1_triple->mutable_subject();
  auto bgp_1_predicate = bgp_1_triple->mutable_predicate();
  auto bgp_1_object = bgp_1_triple->mutable_object();
  bgp_1_subject->set_term_type(proto_msg::VARIABLE);
  bgp_1_subject->set_term_value("?x");
  bgp_1_predicate->set_term_type(proto_msg::IRI);
  bgp_1_predicate->set_term_value("<has_ref>");
  bgp_1_object->set_term_type(proto_msg::VARIABLE);
  bgp_1_object->set_term_value("?y");

  auto bgp_2_triple = bgp2->mutable_triple()->Add();
  auto bgp_2_subject = bgp_2_triple->mutable_subject();
  auto bgp_2_predicate = bgp_2_triple->mutable_predicate();
  auto bgp_2_object = bgp_2_triple->mutable_object();
  bgp_2_subject->set_term_type(proto_msg::VARIABLE);
  bgp_2_subject->set_term_value("?y");
  bgp_2_predicate->set_term_type(proto_msg::IRI);
  bgp_2_predicate->set_term_value("<has_integer>");
  bgp_2_object->set_term_type(proto_msg::VARIABLE);
  bgp_2_object->set_term_value("?z");
  auto result = QueryProcTests2Fixture::cache->run_query(tree, time_control)
                    ->as_query_result_original();
  ASSERT_EQ(result->table().data.size(), values1.size());
}

TEST_F(QueryProcTests2Fixture, problematic_query_2_c1) {
  auto prob_q2 = generate_problematic_query_2();

  add_problematic_query_2_input();

  auto query_result =
      cache->run_query(prob_q2, time_control)->as_query_result_original();

  ASSERT_EQ(query_result->table().data.size(), 1000);
}

TEST_F(QueryProcTests2Fixture, problematic_query_2_c2) {
  auto prob_q2 = generate_problematic_query_2();

  add_problematic_query_2_input();
  add_problematic_query_2_cond_input(1500);

  auto query_result =
      cache->run_query(prob_q2, time_control)->as_query_result_original();

  ASSERT_EQ(query_result->table().data.size(), 500);
}

TEST_F(QueryProcTests2Fixture, problematic_query_3_c1) {
  auto query = generate_problematic_query_3();

  add_problematic_query_3_input();

  auto query_result =
      cache->run_query(query, time_control)->as_query_result_original();
  ASSERT_EQ(query_result->table().data.size(), std::min(pq3_sz, 1000));
}
TEST_F(QueryProcTests2Fixture, problematic_query_3_c2) {
  auto query = generate_problematic_query_3();

  add_problematic_query_3_input();
  add_problematic_query_3_input_not_exist(pq3_sz / 2);

  auto query_result =
      cache->run_query(query, time_control)->as_query_result_original();

  ASSERT_EQ(query_result->table().data.size(), std::min(pq3_sz / 2, 1000));
}

TEST_F(QueryProcTests2Fixture, can_process_table_in_query_1) {
  proto_msg::SparqlTree tree;
  auto *extend_node = tree.mutable_root()->mutable_extend_node();
  auto *table_node = extend_node->mutable_node()->mutable_table_node();

  table_node->mutable_vars()->Add("?x");
  constexpr int table_size = 10;
  std::set<int> expected_set;
  for (size_t i = 0; i < table_size; i++) {
    auto *rdf_term = table_node->mutable_rows()->Add()->mutable_values()->Add();
    rdf_term->set_term_type(proto_msg::TermType::LITERAL);
    rdf_term->set_term_value("\"" + std::to_string(i) + "\"^^xsd:integer");
    expected_set.insert(i);
  }

  auto query_result =
      cache->run_query(tree, time_control)->as_query_result_original();
  auto &table = query_result->table();
  std::set<int> result_set;
  for (auto it = table.data.begin(); it != table.data.end(); ++it) {
    auto id = (*it)[0];
    auto resource = query_result->extract_resource(id);
    auto literal_str =
        ParsingUtils::extract_literal_content_from_string(resource.value);
    auto value = std::stoi(literal_str);
    result_set.insert(value);
  }
  ASSERT_EQ(result_set, expected_set);
}

TEST_F(QueryProcTests2Fixture, can_process_table_in_query_2) {
  proto_msg::SparqlTree tree;
  auto *extend_node = tree.mutable_root()->mutable_extend_node();
  auto *table_node = extend_node->mutable_node()->mutable_table_node();
  table_node->mutable_rows()->Add();

  auto *assignment = extend_node->mutable_assignments()->Add();

  auto *var = assignment->mutable_var();
  var->set_term_type(proto_msg::VARIABLE);
  var->set_term_value("?var1");

  auto *term = assignment->mutable_expr()->mutable_term_node();
  term->set_term_type(proto_msg::LITERAL);
  term->set_basic_type(proto_msg::BasicType::BOOLEAN);
  term->set_term_value("true");

  auto query_result =
      cache->run_query(tree, time_control)->as_query_result_original();
  auto value =
      query_result->extract_resource((*query_result->table().data.begin())[0])
          .value;
  ASSERT_EQ(value, "\"true\"^^xsd:boolean");
}

TEST_F(QueryProcTests2Fixture, can_do_one_var_bgp_testx_1) {

  proto_msg::SparqlTree tree;

  auto *bgp = tree.mutable_root()->mutable_bgp_node();

  auto bgp_triple = bgp->mutable_triple()->Add();
  auto bgp_subject = bgp_triple->mutable_subject();
  auto bgp_predicate = bgp_triple->mutable_predicate();
  auto bgp_object = bgp_triple->mutable_object();
  bgp_subject->set_term_type(proto_msg::IRI);
  bgp_subject->set_term_value("<some_integer_ref2>");
  bgp_predicate->set_term_type(proto_msg::IRI);
  bgp_predicate->set_term_value("<has_integer>");
  bgp_object->set_term_type(proto_msg::VARIABLE);
  bgp_object->set_term_value("?z");
  auto bgp_2_triple = bgp->mutable_triple()->Add();
  auto bgp_2_subject = bgp_2_triple->mutable_subject();
  auto bgp_2_predicate = bgp_2_triple->mutable_predicate();
  auto bgp_2_object = bgp_2_triple->mutable_object();
  bgp_2_subject->set_term_type(proto_msg::IRI);
  bgp_2_subject->set_term_value("<some_integer_refx_1>");
  bgp_2_predicate->set_term_type(proto_msg::IRI);
  bgp_2_predicate->set_term_value("<has_integer>");
  bgp_2_object->set_term_type(proto_msg::VARIABLE);
  bgp_2_object->set_term_value("?z");

  auto qval = "\"" + std::to_string(400) + "\"^^xsd:integer";

  auto res_subj =
      RDFResource("<some_integer_refx_1>", RDFResourceType::RDF_TYPE_IRI);
  auto res_pred = RDFResource("<has_integer>", RDFResourceType::RDF_TYPE_IRI);
  auto res_object = RDFResource(qval, RDFResourceType::RDF_TYPE_LITERAL);
  pcm->add_triple(RDFTripleResource(
      RDFResource(res_subj), RDFResource(res_pred), RDFResource(res_object)));

  auto query_result =
      cache->run_query(tree, time_control)->as_query_result_original();

  ASSERT_EQ(query_result->table().data.size(), 1);
}

TEST_F(QueryProcTests2Fixture,
       can_handle_var_reference_in_sequence_reversed_order_1) {

  auto query = generate_problematic_query_4();
  add_problematic_query_4_input();
  auto query_result =
      cache->run_query(query, time_control)->as_query_result_original();
  ASSERT_EQ(query_result->table().data.size(), pq4_sz / 2);
}

TEST_F(QueryProcTests2Fixture, problematic_query5) {

  auto query = generate_problematic_query_5();
  add_problematic_query_5_input();

  auto result =
      cache->run_query(query, time_control)->as_query_result_original();

  ASSERT_EQ(result->table().data.size(), pq5_sz);
}

int main(int argc, char **argv) {
  ::testing::InitGoogleTest(&argc, argv);
  int result = RUN_ALL_TESTS();
  google::protobuf::ShutdownProtobufLibrary();
  return result;
}
