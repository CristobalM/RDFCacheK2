//
// Created by Cristobal Miranda on 14-01-23.
//
#include "k2tree/K2TreeMixed.hpp"
#include "manager/PCMFactory.hpp"
#include "mock_structures/FHMock.hpp"
#include "mock_structures/StringIOStream.hpp"
#include "mock_structures/StringOStream.hpp"
#include "nodeids/NodeIdsManagerFactory.hpp"
#include "nodeids/NodeIdsManagerImpl.hpp"
#include "nodeids/node_ids_constants.hpp"
#include "random_dataset_generation.hpp"
#include <gtest/gtest.h>

using namespace k2cache;

static void
can_create_triple_ids_in_range_common_testing(uint64_t triples_num,
                                              uint64_t resources_num) {
  K2TreeConfig config{};
  config.treedepth = 32;
  config.cut_depth = 10;
  config.max_node_count = 128;
  auto trees_data = std::make_shared<std::string>();
  auto node_ids_data = std::make_shared<std::string>();
  {
    StringIOStream tmp_stream(std::make_shared<std::string>(),
                              std::ios::trunc | std::ios::binary);
    StringOStream trees_stream(trees_data, std::ios::trunc | std::ios::binary);
    StringOStream node_ids_stream(node_ids_data,
                                  std::ios::trunc | std::ios::binary);

    generate_random_dataset(config, triples_num, resources_num, trees_stream,
                            tmp_stream, node_ids_stream);
    // trees_stream.flush();
    // node_ids_stream.flush();
  }

  auto trees_fh = std::make_unique<FHMock>(trees_data);
  auto node_ids_fh = std::make_unique<FHMock>(node_ids_data);
  auto mapped_node_ids_fh =
      std::make_unique<FHMock>(std::make_shared<std::string>());
  auto logs_fh = std::make_unique<FHMock>(std::make_shared<std::string>());
  auto logs_counter_fh =
      std::make_unique<FHMock>(std::make_shared<std::string>());

  auto pcm = PCMFactory::create(std::move(trees_fh));
  auto nim = NodeIdsManagerFactory::create(
      std::move(node_ids_fh), std::move(mapped_node_ids_fh), std::move(logs_fh),
      std::move(logs_counter_fh));

  auto &seq = dynamic_cast<NodeIdsManagerImpl &>(*nim).get_nodes_sequence();

  ASSERT_EQ(seq.get_values().size(), resources_num);
  ASSERT_EQ(seq.get_last_assigned(), resources_num - 1);

  const auto &predicate_ids =
      pcm->get_predicates_index_cache().get_predicates_ids();
  for (auto p : predicate_ids) {
    auto fetch_result = pcm->get_predicates_index_cache().fetch_k2tree(p);
    auto &tree = fetch_result.get_mutable();
    auto scanner = tree.create_full_scanner();
    while (scanner->has_next()) {
      auto point = scanner->next();
      ASSERT_GE(point.first, 0);
      ASSERT_GE(point.second, 0);
      ASSERT_LT(point.first, resources_num);
      ASSERT_LT(point.second, resources_num);
      int err_code;
      auto real_first = nim->get_real_id(point.first, &err_code);
      ASSERT_EQ(err_code, (int)NidsErrCode::SUCCESS_ERR_CODE);
      auto real_second = nim->get_real_id(point.second, &err_code);
      ASSERT_EQ(err_code, (int)NidsErrCode::SUCCESS_ERR_CODE);
      ASSERT_GT(real_first, 0);
      ASSERT_GT(real_second, 0);
      ASSERT_LE(real_first, 1ULL << 63ULL);
      ASSERT_LE(real_second, 1ULL << 63ULL);
    }
  }
}

TEST(random_dataset_gen_test, can_create_triple_ids_in_range) {
  struct TestProps {
    uint64_t triples_num;
    uint64_t resources_num;
    uint64_t total_repetitions;
  };
  auto test_props = std::vector<TestProps>{
      {1'000'000, 30'000, 1}, {10'000, 5'000, 10}, {10'000, 50'000, 10},
      {1'000, 50'000, 10},    {100, 5000, 100},
  };
  for (auto t : test_props) {
    for (uint64_t i = 0; i < t.total_repetitions; i++) {
      can_create_triple_ids_in_range_common_testing(t.triples_num,
                                                    t.resources_num);
    }
  }
}
