//
// Created by Cristobal Miranda, 2020
//
#include <cmath>
#include <iostream>
#include <memory>
#include <random>

#include <gtest/gtest.h>

#include <sparql_tree.pb.h>

#include <CacheReplacement.hpp>
#include <Cache.hpp>
#include <EmptyISDManager.hpp>
#include <PredicatesCacheManager.hpp>
#include <RDFTriple.hpp>
#include <SDEntitiesMapping.hpp>

#include <StringDictionaryHASHRPDACBlocks.h>

#include <SDBuilder.hpp>

#include <InMemoryCacheSettings.hpp>

static std::vector<std::vector<std::string>>
translate_table(ResultTable &input_table, Cache &cache) {
  std::vector<std::vector<std::string>> translated_table;
  for (auto &row : input_table.data) {
    std::vector<std::string> translated_row;
    for (auto col : row) {
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

  auto pcm = std::make_shared<PredicatesCacheManager>(
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


  
  Cache cache(
    pcm,
    CacheReplacement::STRATEGY::LRU,
    100'000,
    static_cast<std::unique_ptr<ICacheSettings>>(std::make_unique<InMemoryCacheSettings>("")));

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

  auto pcm = std::make_shared<PredicatesCacheManager>(
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

  Cache cache(
    pcm,
    CacheReplacement::STRATEGY::LRU,
    100'000,
    static_cast<std::unique_ptr<ICacheSettings>>(std::make_unique<InMemoryCacheSettings>("")));


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

  auto pcm = std::make_shared<PredicatesCacheManager>(
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

  Cache cache(
    pcm,
    CacheReplacement::STRATEGY::LRU,
    100'000,
    static_cast<std::unique_ptr<ICacheSettings>>(std::make_unique<InMemoryCacheSettings>("")));

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

static std::stringstream
ss_input_from_vector_of_strings(std::vector<std::string> &&data) {
  std::stringstream ss;
  while (!data.empty()) {
    auto curr = data.back();
    data.pop_back();
    ss << curr << '\n';
  }
  return ss;
}

static std::string transform_int_to_str_padded(int value, int padding) {
  std::stringstream ss;

  int max_val = std::pow(10, padding) - 1;
  if (value > max_val)
    throw std::runtime_error("value " + std::to_string(value) +
                             " is bigger than max_value " +
                             std::to_string(max_val));

  int next_pow10 = std::ceil(std::log10(value + 1));
  int remaining_zeros = padding - next_pow10;
  for (int i = 0; i < remaining_zeros; i++)
    ss << "0";
  if (value != 0)
    ss << std::to_string(value);
  return ss.str();
}

static std::vector<std::string> generate_vec_strings(const std::string &prefix,
                                                     int amount) {
  std::vector<std::string> result;
  for (int i = 0; i < amount; i++) {
    result.push_back(prefix + "SOME_STRING_" +
                     transform_int_to_str_padded(i, 4));
  }
  return result;
}

static std::vector<std::vector<std::string>>
transform_table_to_string(const ResultTable &table, const Cache &cache) {
  std::vector<std::vector<std::string>> transformed_data;

  for (const auto &row : table.data) {
    std::vector<std::string> current_row;
    for (auto col : row) {
      current_row.push_back(cache.extract_resource(col));
    }
    transformed_data.push_back(std::move(current_row));
  }

  std::sort(transformed_data.begin(), transformed_data.end(),
            [](const std::vector<std::string> &lhs,
               const std::vector<std::string> &rhs) {
              for (size_t i = 0; i < lhs.size(); i++) {
                if (lhs[i] < rhs[i])
                  return true;
                else if (lhs[i] > rhs[i])
                  return false;
              }
              return true;
            });

  return transformed_data;
}

TEST(QueryProcTests, test_bgp_node_4_compact_dicts) {
  auto iris_data = generate_vec_strings("iris", 1000);
  auto tmp_iris_data = iris_data;
  auto ss_input_iris =
      ss_input_from_vector_of_strings(std::move(tmp_iris_data));

  SDInput sd_input_iris;
  sd_input_iris.bucket_size = 128;
  auto sd_iris = SDBuilder(SDBuilder::SDType::PFC, false, sd_input_iris)
                     .build(ss_input_iris);
  std::stringstream sd_iris_ss(std::ios::in | std::ios::out | std::ios::binary);
  sd_iris->save(sd_iris_ss);

  auto blanks_data = generate_vec_strings("blanks", 1000);
  auto tmp_blanks_data = blanks_data;
  auto ss_input_blanks =
      ss_input_from_vector_of_strings(std::move(tmp_blanks_data));

  SDInput sd_input_blanks;
  sd_input_blanks.cut_size = 100'000;
  sd_input_blanks.thread_count = 4;
  auto sd_blanks =
      SDBuilder(SDBuilder::SDType::HRPDACBlocks, false, sd_input_blanks)
          .build(ss_input_blanks);
  std::stringstream sd_blanks_ss(std::ios::in | std::ios::out |
                                 std::ios::binary);
  sd_blanks->save(sd_blanks_ss);

  auto literals_data = generate_vec_strings("literals", 1000);
  auto tmp_literals_data = literals_data;
  auto ss_input_literals =
      ss_input_from_vector_of_strings(std::move(tmp_literals_data));

  SDInput sd_input_literals;
  sd_input_literals.cut_size = 100'000;
  sd_input_literals.thread_count = 4;

  auto sd_literals =
      SDBuilder(SDBuilder::SDType::HRPDACBlocks, false, sd_input_literals)
          .build(ss_input_literals);
  std::stringstream sd_literals_ss(std::ios::in | std::ios::out |
                                   std::ios::binary);
  sd_literals->save(sd_literals_ss);

  sd_iris_ss.seekg(0);
  sd_iris_ss.seekp(0);

  sd_blanks_ss.seekg(0);
  sd_blanks_ss.seekp(0);

  sd_literals_ss.seekg(0);
  sd_literals_ss.seekp(0);

  auto sdent = std::make_unique<
      SDEntitiesMapping<StringDictionaryPFC, StringDictionaryHASHRPDACBlocks,
                        StringDictionaryHASHRPDACBlocks>>(
      sd_iris_ss, sd_blanks_ss, sd_literals_ss);

  std::unordered_set<unsigned long> blank_indexes;
  for (const auto &s : blanks_data) {
    blank_indexes.insert(sdent->blanks_index(s));
  }

  ASSERT_EQ(blank_indexes.size(), blanks_data.size());

  std::unordered_set<unsigned long> iris_indexes;
  for (const auto &s : iris_data) {
    iris_indexes.insert(sdent->iris_index(s));
  }

  ASSERT_EQ(iris_indexes.size(), iris_data.size());

  std::unordered_set<unsigned long> literal_indexes;
  for (const auto &s : literals_data) {
    literal_indexes.insert(sdent->literals_index(s));
  }

  ASSERT_EQ(literal_indexes.size(), literals_data.size());

  std::unordered_set<unsigned long> union_set;

  union_set.insert(iris_indexes.begin(), iris_indexes.end());
  union_set.insert(literal_indexes.begin(), literal_indexes.end());
  union_set.insert(blank_indexes.begin(), blank_indexes.end());

  ASSERT_EQ(union_set.size(), blank_indexes.size() + literal_indexes.size() +
                                  blank_indexes.size());

  ASSERT_TRUE(union_set.find(0) == union_set.end());
  ASSERT_TRUE(union_set.find(1) != union_set.end());
  ASSERT_TRUE(union_set.find(union_set.size()) != union_set.end());
  ASSERT_TRUE(union_set.find(union_set.size() + 1) == union_set.end());

  auto pcm = std::make_shared<PredicatesCacheManager>(std::move(sdent));

  auto predicate_str = iris_data[iris_data.size() / 2 + 1];

  for (unsigned long i = 0; i < iris_data.size(); i++) {
    pcm->add_triple(RDFTripleResource(
        RDFResource(std::string(iris_data[i]), RDFResourceType::RDF_TYPE_IRI),
        RDFResource(std::string(predicate_str), RDFResourceType::RDF_TYPE_IRI),
        RDFResource(std::string(literals_data[i]),
                    RDFResourceType::RDF_TYPE_LITERAL)));
  }

  for (unsigned long i = 0; i < iris_data.size(); i++) {
    ASSERT_TRUE(pcm->has_triple(RDFTripleResource(
        RDFResource(std::string(iris_data[i]), RDFResourceType::RDF_TYPE_IRI),
        RDFResource(std::string(predicate_str), RDFResourceType::RDF_TYPE_IRI),
        RDFResource(std::string(literals_data[i]),
                    RDFResourceType::RDF_TYPE_LITERAL))))
        << "Not found triple (" << iris_data[i] << ", " << predicate_str << ", "
        << literals_data[i] << ")";
  }

  ASSERT_EQ(pcm->get_dyn_dicts().size(), 0);


  Cache cache(
    pcm,
    CacheReplacement::STRATEGY::LRU,
    100'000,
    static_cast<std::unique_ptr<ICacheSettings>>(std::make_unique<InMemoryCacheSettings>("")));

  proto_msg::SparqlTree tree;
  auto *project_node = tree.mutable_root()->mutable_project_node();
  project_node->add_vars("?x");
  project_node->add_vars("?y");
  auto *bgp = project_node->mutable_sub_op()->mutable_bgp_node();
  auto *first_triple = bgp->mutable_triple()->Add();
  auto *first_subject = first_triple->mutable_subject();
  first_subject->set_term_type(proto_msg::TermType::VARIABLE);
  first_subject->set_basic_type(proto_msg::BasicType::STRING);
  first_subject->set_term_value("?x");

  auto *first_predicate = first_triple->mutable_predicate();
  first_predicate->set_term_type(proto_msg::TermType::IRI);
  first_predicate->set_basic_type(proto_msg::BasicType::STRING);
  first_predicate->set_term_value(predicate_str);

  auto *first_object = first_triple->mutable_object();
  first_object->set_term_type(proto_msg::TermType::VARIABLE);
  first_object->set_basic_type(proto_msg::BasicType::STRING);
  first_object->set_term_value("?y");

  auto query_result = cache.run_query(tree);

  auto reverse_var_map = query_result.get_vim().reverse();
  auto &headers = query_result.table().headers;
  auto h0 = reverse_var_map[headers[0]];
  auto h1 = reverse_var_map[headers[1]];

  ASSERT_EQ(h0, "?x");
  ASSERT_EQ(h1, "?y");

  auto transformed_data =
      transform_table_to_string(query_result.table(), cache);

  for (size_t row_i = 0; row_i < transformed_data.size(); row_i++) {
    ASSERT_EQ(transformed_data[row_i].size(), 2);
    ASSERT_EQ(transformed_data[row_i][0], iris_data[row_i]);
    ASSERT_EQ(transformed_data[row_i][1], literals_data[row_i]);
  }

  auto predicate_str2 = iris_data[iris_data.size() / 3];

  auto &pcm2 = cache.get_pcm();

  for (unsigned long i = 0; i < iris_data.size(); i++) {
    pcm2.add_triple(RDFTripleResource(
        RDFResource(std::string(iris_data[i]), RDFResourceType::RDF_TYPE_IRI),
        RDFResource(std::string(predicate_str2), RDFResourceType::RDF_TYPE_IRI),
        RDFResource(std::string(literals_data[i]),
                    RDFResourceType::RDF_TYPE_LITERAL)));
  }

  auto *second_triple = bgp->mutable_triple()->Add();
  auto *second_subject = second_triple->mutable_subject();
  second_subject->set_term_type(proto_msg::TermType::VARIABLE);
  second_subject->set_basic_type(proto_msg::BasicType::STRING);
  second_subject->set_term_value("?x");

  auto *second_predicate = second_triple->mutable_predicate();
  second_predicate->set_term_type(proto_msg::TermType::IRI);
  second_predicate->set_basic_type(proto_msg::BasicType::STRING);
  second_predicate->set_term_value(predicate_str2);

  auto *second_object = second_triple->mutable_object();
  second_object->set_term_type(proto_msg::TermType::VARIABLE);
  second_object->set_basic_type(proto_msg::BasicType::STRING);
  second_object->set_term_value("?y");

  auto query_result_2 = cache.run_query(tree);

  auto reverse_var_map_2 = query_result_2.get_vim().reverse();
  auto &headers_2 = query_result_2.table().headers;
  auto h0_2 = reverse_var_map_2[headers_2[0]];
  auto h1_2 = reverse_var_map_2[headers_2[1]];

  ASSERT_EQ(h0_2, "?x");
  ASSERT_EQ(h1_2, "?y");

  auto transformed_data_2 =
      transform_table_to_string(query_result_2.table(), cache);

  ASSERT_EQ(transformed_data_2.size(), transformed_data.size());

  for (size_t i = 0; i < transformed_data.size(); i++) {
    ASSERT_EQ(transformed_data_2[i].size(), transformed_data[i].size());
    for (size_t j = 0; j < transformed_data[i].size(); j++) {
      ASSERT_EQ(transformed_data_2[i][j], transformed_data[i][j]);
    }
  }

  auto predicate_str3 = iris_data[iris_data.size() / 3 + 1];

  for (unsigned long i = 0; i < iris_data.size()/2; i++) {
      pcm2.add_triple(RDFTripleResource(
    RDFResource(std::string(iris_data[i]), RDFResourceType::RDF_TYPE_IRI),
    RDFResource(std::string(predicate_str3), RDFResourceType::RDF_TYPE_IRI),
    RDFResource(std::string(literals_data[i]),
                RDFResourceType::RDF_TYPE_LITERAL)));
  }

  auto *third_triple = bgp->mutable_triple()->Add();
  auto *third_subject = third_triple->mutable_subject();
  third_subject->set_term_type(proto_msg::TermType::VARIABLE);
  third_subject->set_basic_type(proto_msg::BasicType::STRING);
  third_subject->set_term_value("?x");

  auto *third_predicate = third_triple->mutable_predicate();
  third_predicate->set_term_type(proto_msg::TermType::IRI);
  third_predicate->set_basic_type(proto_msg::BasicType::STRING);
  third_predicate->set_term_value(predicate_str3);

  auto *third_object = third_triple->mutable_object();
  third_object->set_term_type(proto_msg::TermType::VARIABLE);
  third_object->set_basic_type(proto_msg::BasicType::STRING);
  third_object->set_term_value("?y");


  auto query_result_3 = cache.run_query(tree);

  auto transformed_data_3 =
      transform_table_to_string(query_result_3.table(), cache);

  ASSERT_EQ(transformed_data_3.size(), iris_data.size()/2);

  std::unordered_set<std::string> set_big;
  std::unordered_set<std::string> set_small;

  for(const auto & row: transformed_data_2){
    std::stringstream ss;
    for(const auto & s: row) ss << s;
    set_big.insert(ss.str());
  }

  for(const auto & row: transformed_data_3){
    std::stringstream ss;
    for(const auto & s: row) ss << s;
    set_small.insert(ss.str());
  }

  for(const auto & s: set_small){
    ASSERT_TRUE(set_big.find(s) != set_big.end());
  }
}
