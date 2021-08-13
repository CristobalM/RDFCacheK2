#include <gtest/gtest.h>

#include <K2TreeBulkOp.hpp>
#include <PredicatesIndexCacheMD.hpp>
#include <PredicatesIndexFileBuilder.hpp>
#include <google/protobuf/stubs/common.h>
#include <serialization_util.hpp>
#include <string>
#include <triple_external_sort.hpp>
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
static std::pair<PredicatesIndexCacheMD, unsigned long>
build_picmd_single_predicate(unsigned long predicate_id) {

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
    TripleValue(i, predicate_id, i).write_to_file(ss);
    TripleValue(i + 1, predicate_id, i + 1).write_to_file(ss);
    TripleValue(i + 2, predicate_id, i + 2).write_to_file(ss);
  }
  ss.seekg(0);
  out->seekp(0);

  PredicatesIndexFileBuilder::build(ss, *out, tmp, config);

  return {PredicatesIndexCacheMD(std::move(out)), sz};
}

static std::pair<PredicatesIndexCacheMD, unsigned long>
build_picmd_2(unsigned long predicate_id) {

  K2TreeConfig config;
  config.cut_depth = 10;
  config.max_node_count = 256;
  config.treedepth = 32;

  std::stringstream ss;
  auto out = std::make_unique<std::stringstream>();
  std::stringstream tmp;
  unsigned long sz = 10000;
  write_u64(ss, sz * 10);
  for (unsigned long i = 1; i <= sz; i++) {
    for (unsigned long j = i; j < 10; j++) {
      TripleValue(i, predicate_id, j).write_to_file(ss);
    }
  }
  ss.seekg(0);
  out->seekp(0);

  PredicatesIndexFileBuilder::build(ss, *out, tmp, config);

  return {PredicatesIndexCacheMD(std::move(out)), sz};
}

TEST(predicates_metadata_serialization, same_k2tree_as_non_serialized) {
  const unsigned long predicate_id = 123456;
  auto [pc, sz] = build_picmd_2(predicate_id);

  K2TreeMixed non_serialized(32, 256, 10);
  K2TreeBulkOp bulk_op_non_serialized(non_serialized);
  for (unsigned long i = 1; i <= sz; i++) {
    for (unsigned long j = i; j < 10; j++) {
      bulk_op_non_serialized.insert(i, j);
    }
  }

  auto fetch_result = pc.fetch_k2tree(predicate_id);
  const auto &deserialized = fetch_result.get();

  std::stringstream ss_des;
  auto written_sz_des = deserialized.write_to_ostream(ss_des);

  std::stringstream ss_nonser;
  auto written_sz_nonser = non_serialized.write_to_ostream(ss_nonser);

  ASSERT_EQ(written_sz_des, written_sz_nonser);

  auto s_des = ss_des.str();
  auto s_nonser = ss_nonser.str();
  ASSERT_EQ(s_des.size(), s_nonser.size());
  ASSERT_EQ(s_des, s_nonser);

  ASSERT_TRUE(deserialized.same_as(non_serialized));
  ASSERT_TRUE(non_serialized.same_as(deserialized));

  ASSERT_TRUE(deserialized.has_valid_structure());
  ASSERT_TRUE(non_serialized.has_valid_structure());
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
    auto current_metadata = metadata_map.at(predicate_id);
    ASSERT_EQ(current_metadata.predicate_id, predicate_id);
    ASSERT_GT(current_metadata.tree_offset, current_offset)
        << "failed offset check at predicate " << predicate_id;
    current_offset = current_metadata.tree_offset;

    ASSERT_GT(current_metadata.predicate_id, last_predicate)
        << "failed predicate_id check at predicate " << predicate_id;
    last_predicate = current_metadata.predicate_id;
  }

  for (unsigned long i = sz; i > 0; i--) {
    auto fetch_result = pc.fetch_k2tree(i);
    auto &k2tree = fetch_result.get_mutable();
    K2TreeBulkOp bulk_op_curr(k2tree);

    ASSERT_TRUE(bulk_op_curr.has(i, i));
    ASSERT_TRUE(bulk_op_curr.has(i + 1, i + 1));
    ASSERT_TRUE(bulk_op_curr.has(i + 2, i + 2));
    ASSERT_FALSE(bulk_op_curr.has(i, i + 1));
    ASSERT_EQ(k2tree.get_tree_depth(), 32);

    ASSERT_EQ(k2tree.size(), 3);
    ASSERT_TRUE(k2tree.has_valid_structure(bulk_op_curr.get_stw()));
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
    auto fetch_result = pc.fetch_k2tree(i);
    auto &k2tree = fetch_result.get_mutable();
    K2TreeBulkOp bulk_op(k2tree);
    ASSERT_TRUE(bulk_op.has(i, i));
    ASSERT_FALSE(bulk_op.has(i, i + 1));
    ASSERT_TRUE(k2tree.has_valid_structure(bulk_op.get_stw()));
  }

  for (unsigned long i = 20'000; i < 30'000; i++) {
    auto fetch_result = pc.fetch_k2tree(i);
    auto &k2tree = fetch_result.get_mutable();
    K2TreeBulkOp bulk_op(k2tree);
    ASSERT_TRUE(bulk_op.has(i, i));
    ASSERT_FALSE(bulk_op.has(i, i + 1));
    ASSERT_TRUE(k2tree.has_valid_structure(bulk_op.get_stw()));
  }
}

TEST(predicates_metadata_serialization, can_store_predicate_size_in_memory) {
  unsigned long predicate_id = 12345;
  auto [pc, sz] = build_picmd_single_predicate(predicate_id);
  auto fetched = pc.fetch_k2tree(predicate_id);
  auto &k2tree = fetched.get_mutable();

  auto k2tree_stats = k2tree.k2tree_stats();
  const auto &metadata = pc.get_metadata().get_map().at(predicate_id);
  auto stored_in_memory_size = metadata.tree_size_in_memory;
  auto calculated_in_memory_size = k2tree_stats.total_bytes;
  ASSERT_EQ(stored_in_memory_size, calculated_in_memory_size);
}

int main(int argc, char **argv) {
  ::testing::InitGoogleTest(&argc, argv);
  int result = RUN_ALL_TESTS();
  google::protobuf::ShutdownProtobufLibrary();
  return result;
}
