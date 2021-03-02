#include <gtest/gtest.h>

#include <PredicatesIndexCacheMD.hpp>
#include <PredicatesIndexFileBuilder.hpp>
#include <external_sort.hpp>
#include <serialization_util.hpp>
#include <sstream>
#include <string>
#include <utility>

static std::pair<PredicatesIndexCacheMD, unsigned long> build_picmd() {

  K2TreeConfig config;
  config.cut_depth = 10;
  config.max_node_count = 256;
  config.treedepth = 32;

  std::stringstream ss;
  auto out = std::make_unique<std::stringstream>();
  std::stringstream tmp;
  unsigned long sz = 10000;
  write_u64(ss, 3 * sz);
  for (unsigned long i = 1; i <= sz; i++) {
    TripleValue(i, i, i).write_to_file(ss);
    TripleValue(i + 1, i, i + 1).write_to_file(ss);
    TripleValue(i + 2, i, i + 2).write_to_file(ss);
  }
  ss.seekg(0);
  out->seekp(0);

  PredicatesIndexFileBuilder::build(ss, *out, tmp, config);

  return {PredicatesIndexCacheMD(std::move(out)), sz};
}

TEST(predicates_metadata_serialization, can_create_save_and_retrieve) {

  auto [pc, sz] = build_picmd();

  auto &metadata = pc.get_metadata();
  ASSERT_EQ(metadata.get_predicates_count(), sz);

  auto config = metadata.get_config();

  ASSERT_EQ(config.cut_depth, 10);
  ASSERT_EQ(config.max_node_count, 256);
  ASSERT_EQ(config.treedepth, 32);

  const auto &metadata_map = metadata.get_map();

  unsigned long current_offset = 0;
  unsigned long last_predicate = 0;
  for (auto predicate_id : metadata.get_ids_vector()) {
    auto metadata = metadata_map.at(predicate_id);
    ASSERT_EQ(metadata.predicate_id, predicate_id);
    ASSERT_GT(metadata.tree_offset, current_offset)
        << "failed offset check at predicate " << predicate_id;
    current_offset = metadata.tree_offset;

    ASSERT_GT(metadata.predicate_id, last_predicate)
        << "failed predicate_id check at predicate " << predicate_id;
    last_predicate = metadata.predicate_id;
  }

  for (unsigned long i = sz; i > 0; i--) {
    auto &k2tree = pc.fetch_k2tree(i);

    ASSERT_TRUE(k2tree.has(i, i));
    ASSERT_TRUE(k2tree.has(i + 1, i + 1));
    ASSERT_TRUE(k2tree.has(i + 2, i + 2));
    ASSERT_FALSE(k2tree.has(i, i + 1));
    ASSERT_EQ(k2tree.get_tree_depth(), 32);

    ASSERT_EQ(k2tree.size(), 3);
  }
}

TEST(predicates_metadata_serialization, can_sync_with_external) {
  auto [pc, sz] = build_picmd();

  for (unsigned long i = 20'000; i < 30'000; i++) {
    pc.insert_point(i, i, i);
  }

  auto new_data = std::make_unique<std::stringstream>();
  pc.sync_to_stream(*new_data);
  pc.replace_istream(std::move(new_data));

  for (unsigned long i = sz; i > 0; i--) {
    auto &k2tree = pc.fetch_k2tree(i);
    ASSERT_TRUE(k2tree.has(i, i));
    ASSERT_FALSE(k2tree.has(i, i + 1));
  }

  for (unsigned long i = 20'000; i < 30'000; i++) {
    auto &k2tree = pc.fetch_k2tree(i);
    ASSERT_TRUE(k2tree.has(i, i));
    ASSERT_FALSE(k2tree.has(i, i + 1));
  }
}
