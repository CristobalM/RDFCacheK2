#include <gtest/gtest.h>


#include <serialization_util.hpp>
#include <string>
#include <triple_external_sort.hpp>
#include <utility>
#include <sstream>
#include <cstdint>

#include "builder/PredicatesIndexFileBuilder.hpp"
#include "cache_test_util.hpp"
#include "k2tree/K2TreeBulkOp.hpp"
#include "manager/PredicatesIndexCacheMD.hpp"
#include "mock_structures/FHMock.hpp"


using namespace k2cache;

static std::pair<PredicatesIndexCacheMD, uint64_t> build_picmd_wbuild_fun(
    DataHolders &h,
    const std::function<PredicatesCacheMetadata(
        std::istream&, std::ostream&, std::iostream&, K2TreeConfig)> &build_fun,
    uint64_t sz=10000
    ) {

  K2TreeConfig config;
  config.cut_depth = 10;
  config.max_node_count = 256;
  config.treedepth = 32;

  std::stringstream ss;

  std::stringstream out;
  std::stringstream tmp;
  write_u64(ss, 3 * sz);
  for (uint64_t i = 1; i <= sz; i++) {
    TripleValue(i, i, i).write_to_file(ss);
    TripleValue(i + 1, i, i + 1).write_to_file(ss);
    TripleValue(i + 2, i, i + 2).write_to_file(ss);
  }
  ss.seekg(0);
  out.seekp(0);

  build_fun(ss, out, tmp, config);

  h.pcm_h.data = std::make_shared<std::string>(out.str());

  auto frw_handler = std::make_unique<FHMock>(h.pcm_h.data);

  return {PredicatesIndexCacheMD(std::move(frw_handler)), sz};
}


static std::pair<PredicatesIndexCacheMD, uint64_t> build_picmd(DataHolders &h) {
  return build_picmd_wbuild_fun(h,
                                PredicatesIndexFileBuilder::build);
}


static std::pair<PredicatesIndexCacheMD, uint64_t>
build_picmd_single_predicate(uint64_t predicate_id, DataHolders &h) {

  K2TreeConfig config;
  config.cut_depth = 10;
  config.max_node_count = 256;
  config.treedepth = 32;

  std::stringstream ss;
  std::stringstream out;
  std::stringstream tmp;
  uint64_t sz = 10000;
  write_u64(ss, 3 * sz);
  for (uint64_t i = 1; i <= sz; i++) {
    TripleValue(i, predicate_id, i).write_to_file(ss);
    TripleValue(i + 1, predicate_id, i + 1).write_to_file(ss);
    TripleValue(i + 2, predicate_id, i + 2).write_to_file(ss);
  }
  ss.seekg(0);
  out.seekp(0);

  PredicatesIndexFileBuilder::build(ss, out, tmp, config);

  h.pcm_h.data = std::make_shared<std::string>(out.str());

  auto frw_handler = std::make_unique<FHMock>(h.pcm_h.data);

  return {PredicatesIndexCacheMD(std::move(frw_handler)), sz};
}

static std::pair<std::unique_ptr<PredicatesIndexCacheMD>, uint64_t>
build_picmd_2(uint64_t predicate_id) {
  K2TreeConfig config;
  config.cut_depth = 10;
  config.max_node_count = 256;
  config.treedepth = 32;
  const auto sz = 10000UL;
  auto cpd = build_pred_data_sz(config, predicate_id, sz);
  return {cpd.get_picmd(), sz};
  //  K2TreeConfig config;
  //  config.cut_depth = 10;
  //  config.max_node_count = 256;
  //  config.treedepth = 32;
  //
  //  std::stringstream ss;
  //  auto out = std::make_unique<std::stringstream>();
  //  std::stringstream tmp;
  //  uint64_t sz = 10000;
  //  write_u64(ss, sz * 10);
  //  for (uint64_t i = 1; i <= sz; i++) {
  //    for (uint64_t j = i; j < 10; j++) {
  //      TripleValue(i, predicate_id, j).write_to_file(ss);
  //    }
  //  }
  //  ss.seekg(0);
  //  out->seekp(0);
  //
  //  PredicatesIndexFileBuilder::build(ss, *out, tmp, config);
  //
  //  auto frw_handler = std::make_unique<FHMock>(out->str());
  //
  //  return {PredicatesIndexCacheMD(std::move(frw_handler)), sz};
}

TEST(predicates_metadata_serialization, same_k2tree_as_non_serialized) {
  const uint64_t predicate_id = 123456;
  auto [pc, sz] = build_picmd_2(predicate_id);

  K2TreeMixed non_serialized(32, 256, 10);
  K2TreeBulkOp bulk_op_non_serialized(non_serialized);
  for (uint64_t i = 1; i <= sz; i++) {
    for (uint64_t j = i; j < 10; j++) {
      bulk_op_non_serialized.insert(i, j);
    }
  }

  auto fetch_result = pc->fetch_k2tree(predicate_id);
  ASSERT_TRUE(fetch_result.exists());
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

  ASSERT_TRUE(deserialized.identical_structure_as(non_serialized));
  ASSERT_TRUE(non_serialized.identical_structure_as(deserialized));

  ASSERT_TRUE(deserialized.has_valid_structure());
  ASSERT_TRUE(non_serialized.has_valid_structure());
}

TEST(predicates_metadata_serialization, can_create_save_and_retrieve) {
  DataHolders h;
  auto [pc, sz] = build_picmd(h);

  auto &metadata = pc.get_metadata();
  ASSERT_EQ(metadata.get_predicates_count(), sz);

  auto config = metadata.get_config();

  ASSERT_EQ(config.cut_depth, 10);
  ASSERT_EQ(config.max_node_count, 256);
  ASSERT_EQ(config.treedepth, 32);

  const auto &metadata_map = metadata.get_map();

  uint64_t current_offset = 0;
  uint64_t last_predicate = 0;
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

  for (uint64_t i = sz; i > 0; i--) {
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
  DataHolders h;
  auto [pc, sz] = build_picmd(h);

  for (uint64_t i = 20'000; i < 30'000; i++) {
    pc.insert_point(i, i, i);
  }

  pc.sync_to_persistent();

  for (uint64_t i = sz; i > 0; i--) {
    auto fetch_result = pc.fetch_k2tree(i);
    auto &k2tree = fetch_result.get_mutable();
    K2TreeBulkOp bulk_op(k2tree);
    ASSERT_TRUE(bulk_op.has(i, i));
    ASSERT_FALSE(bulk_op.has(i, i + 1));
    ASSERT_TRUE(k2tree.has_valid_structure(bulk_op.get_stw()));
  }

  for (uint64_t i = 20'000; i < 30'000; i++) {
    auto fetch_result = pc.fetch_k2tree(i);
    auto &k2tree = fetch_result.get_mutable();
    K2TreeBulkOp bulk_op(k2tree);
    ASSERT_TRUE(bulk_op.has(i, i));
    ASSERT_FALSE(bulk_op.has(i, i + 1));
    ASSERT_TRUE(k2tree.has_valid_structure(bulk_op.get_stw()));
  }
}

TEST(predicates_metadata_serialization, can_store_predicate_size_in_memory) {
  uint64_t predicate_id = 12345;
  DataHolders h;
  auto [pc, sz] = build_picmd_single_predicate(predicate_id, h);
  auto fetched = pc.fetch_k2tree(predicate_id);
  auto &k2tree = fetched.get_mutable();

  auto k2tree_stats = k2tree.k2tree_stats();
  const auto &metadata = pc.get_metadata().get_map().at(predicate_id);
  auto stored_in_memory_size = metadata.tree_size_in_memory;
  auto calculated_in_memory_size = k2tree_stats.total_bytes;
  ASSERT_EQ(stored_in_memory_size, calculated_in_memory_size);
}

TEST(predicates_metadata_serialization, same_perf_original_vs_newer_virt_build_fun){
  DataHolders h;

  const uint64_t total_samples = 10;
  uint64_t sum_orig = 0;
  uint64_t sum_new = 0;
  const auto measurement_unit = "microsecs";
  for(uint64_t i = 0; i < total_samples; i++){

    auto start = std::chrono::high_resolution_clock::now();

    auto [pc, sz] =
        build_picmd_wbuild_fun(h, PredicatesIndexFileBuilder::build_orig);
    auto stop = std::chrono::high_resolution_clock::now();
    auto duration_orig =
        std::chrono::duration_cast<std::chrono::microseconds>(stop - start);

    start = std::chrono::high_resolution_clock::now();

    auto [pc2, sz2] =
        build_picmd_wbuild_fun(h, PredicatesIndexFileBuilder::build);
    stop = std::chrono::high_resolution_clock::now();
    auto duration_new =
        std::chrono::duration_cast<std::chrono::microseconds>(stop - start);
    std::cout << "Duration new " << duration_new.count() << " " << measurement_unit << std::endl;
    std::cout << "Duration orig " << duration_orig.count() << " " << measurement_unit <<  std::endl;

    sum_new += duration_new.count();
    sum_orig += duration_orig.count();
  }

  auto avg_new = sum_new / total_samples;
  auto avg_orig = sum_orig / total_samples;

  std::cout << "Avg new: " << avg_new << " " << measurement_unit <<  std::endl;
  std::cout << "Avg orig: " << avg_orig << " " << measurement_unit <<  std::endl;
  std::cout << "Avg orig - new: " << avg_orig - avg_new << " " << measurement_unit << std::endl;
  std::cout << "Avg diff (orig - new): " << (sum_orig - sum_new) / total_samples << " " << measurement_unit << std::endl;

  ASSERT_LE(std::abs((int64_t)avg_new - (int64_t)avg_orig), 10000);
}

TEST(predicates_metadata_serialization, same_perf_original_vs_newer_virt_build_fun_reversed){
  DataHolders h;

  const int64_t total_samples = 10;
  int64_t sum_orig = 0;
  int64_t sum_new = 0;
  const auto measurement_unit = "microsecs";
  for(uint64_t i = 0; i < total_samples; i++){

    auto start = std::chrono::high_resolution_clock::now();

    auto [pc, sz] =
        build_picmd_wbuild_fun(h, PredicatesIndexFileBuilder::build);
    auto stop = std::chrono::high_resolution_clock::now();
    auto duration_new =
        std::chrono::duration_cast<std::chrono::microseconds>(stop - start);

    start = std::chrono::high_resolution_clock::now();

    auto [pc2, sz2] =
        build_picmd_wbuild_fun(h, PredicatesIndexFileBuilder::build_orig);
    stop = std::chrono::high_resolution_clock::now();
    auto duration_orig =
        std::chrono::duration_cast<std::chrono::microseconds>(stop - start);
    std::cout << "Duration new " << duration_new.count() << " " << measurement_unit << std::endl;
    std::cout << "Duration orig " << duration_orig.count() << " " << measurement_unit <<  std::endl;

    sum_new += duration_new.count();
    sum_orig += duration_orig.count();
  }

  int64_t avg_new = (int64_t)sum_new / total_samples;
  int64_t avg_orig = (int64_t)sum_orig / total_samples;

  std::cout << "Avg new: " << avg_new << " " << measurement_unit <<  std::endl;
  std::cout << "Avg orig: " << avg_orig << " " << measurement_unit <<  std::endl;
  std::cout << "Avg orig - new: " << avg_orig - avg_new << " " << measurement_unit << std::endl;
  std::cout << "Avg diff (orig - new): " << (sum_orig - sum_new) / total_samples << " " << measurement_unit << std::endl;

  ASSERT_LE(std::abs((int64_t)avg_new - (int64_t)avg_orig), 10000);
}

TEST(predicates_metadata_serialization, benchmark_orig) {
  DataHolders h;
  auto [pc, sz] =
      build_picmd_wbuild_fun(h, PredicatesIndexFileBuilder::build_orig, 100000);
  ASSERT_GT(sz, 0);
}

TEST(predicates_metadata_serialization, benchmark_new) {
  DataHolders h;
  auto [pc, sz] =
      build_picmd_wbuild_fun(h, PredicatesIndexFileBuilder::build, 100000);
  ASSERT_GT(sz, 0);
}

int main(int argc, char **argv) {
  ::testing::InitGoogleTest(&argc, argv);
  int result = RUN_ALL_TESTS();
  google::protobuf::ShutdownProtobufLibrary();
  return result;
}
