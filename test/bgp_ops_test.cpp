//
// Created by cristobal on 30-05-23.
//
#include <gtest/gtest.h>
#include <TimeControl.hpp>

#include <memory>

#include "k2tree/K2TreeBulkOp.hpp"
#include "k2tree/K2TreeMixed.hpp"
#include "query_processing/iterators/bgpops/OneVarCPBGPOp.hpp"
#include "query_processing/iterators/bgpops/OneVarIntersectBGPOp.hpp"
#include "query_processing/iterators/bgpops/TwoVarCProductBGPOp.hpp"

using namespace k2cache;


TEST(bgp_ops_test, one_var_cp_bgpop_objvar_test){
  TimeControl tc(1000, std::chrono::milliseconds(30000));
  K2TreeConfig config{};
  config.cut_depth = 10;
  config.max_node_count = 128;
  config.treedepth = 32;
  auto k2tree = std::make_unique<K2TreeMixed>(config);
  K2TreeBulkOp bulk_insert(*k2tree);
  for(int i = 100; i < 200; i++){
    for(int j = i*i; j < i*i+100; j++){
      bulk_insert.insert(i , j);
    }
  }
  auto scanner =k2tree->create_band_scanner(100, k2cache::K2TreeScanner::COLUMN_BAND_TYPE);
  auto op = std::make_unique<OneVarCPBGPOp<BGPOp::OBJECT_VAR>>(
      std::move(scanner), 0, tc);
  std::vector<unsigned long> row_to_fill(1, 0);
  bool done = false;
  int curr = 0;
  do {
    auto run_result = op->run(row_to_fill);
    done = run_result.scan_done;
    ASSERT_TRUE(run_result.valid_value);
    ASSERT_EQ(row_to_fill[0], 100*100 + curr);
    curr++;
  } while (!done);
}

TEST(bgp_ops_test, one_var_cp_bgpop_subjvar_test){
  TimeControl tc(1000, std::chrono::milliseconds(30000));
  K2TreeConfig config{};
  config.cut_depth = 10;
  config.max_node_count = 128;
  config.treedepth = 32;
  auto k2tree = std::make_unique<K2TreeMixed>(config);
  K2TreeBulkOp bulk_insert(*k2tree);
  for(int i = 100; i < 200; i++){
    for(int j = i*i; j < i*i+100; j++){
      bulk_insert.insert(j , i);
    }
  }
  auto scanner =k2tree->create_band_scanner(100, k2cache::K2TreeScanner::ROW_BAND_TYPE);
  auto op = std::make_unique<OneVarCPBGPOp<BGPOp::SUBJECT_VAR>>(
      std::move(scanner), 0, tc);
  std::vector<unsigned long> row_to_fill(1, 0);
  bool done = false;
  int curr = 0;
  do {
    auto run_result = op->run(row_to_fill);
    done = run_result.scan_done;
    ASSERT_TRUE(run_result.valid_value);
    ASSERT_EQ(row_to_fill[0], 100*100 + curr);
    curr++;
  } while (!done);
}

TEST(bgp_ops_test, one_var_intersect_bgp_op_empty_test){
  TimeControl tc(1000, std::chrono::milliseconds(30000));
  K2TreeConfig config{};
  config.cut_depth = 10;
  config.max_node_count = 128;
  config.treedepth = 32;
  auto k2tree = std::make_unique<K2TreeMixed>(config);
  K2TreeBulkOp bulk_insert(*k2tree);
  for(int i = 100; i < 200; i++){
    for(int j = i*i; j < i*i+100; j++){
      bulk_insert.insert(i , j);
    }
  }

  auto scanner =k2tree->create_band_scanner(100, k2cache::K2TreeScanner::COLUMN_BAND_TYPE);
  auto op = std::make_unique<OneVarIntersectBGPOp<BGPOp::OBJECT_VAR>>(
      std::move(scanner), 0, tc);
  std::vector<unsigned long> row_to_fill(1, 0);
  bool done = false;
  int curr = 0;
  do {
    auto run_result = op->run(row_to_fill);
    done = run_result.scan_done;
    ASSERT_FALSE(run_result.valid_value);
    curr++;
  } while (!done);
}

TEST(bgp_ops_test, one_var_intersect_bgp_op_full_objvar_test){
  TimeControl tc(1000, std::chrono::milliseconds(30000));
  K2TreeConfig config{};
  config.cut_depth = 10;
  config.max_node_count = 128;
  config.treedepth = 32;
  auto k2tree = std::make_unique<K2TreeMixed>(config);
  K2TreeBulkOp bulk_insert(*k2tree);
  for(int i = 100; i < 200; i++){
    for(int j = i*i; j < i*i+100; j++){
      bulk_insert.insert(i , j);
    }
  }

  auto scanner =k2tree->create_band_scanner(100, k2cache::K2TreeScanner::COLUMN_BAND_TYPE);
  auto op = std::make_unique<OneVarIntersectBGPOp<BGPOp::OBJECT_VAR>>(
      std::move(scanner), 0, tc);
  std::vector<unsigned long> row_to_fill(1, 100*100 + 50);
  bool done = false;
  int curr = 0;
  unsigned long match_value = 0;
  int valid_count = 0;
  do {
    auto run_result = op->run(row_to_fill);
    done = run_result.scan_done;
//    ASSERT_TRUE(run_result.valid_value) << "failed at curr = " << curr;
    curr++;
    if(run_result.valid_value){
      match_value = row_to_fill[0];
      valid_count++;
    }
  } while (!done);
  ASSERT_EQ(valid_count, 1);
  ASSERT_EQ(match_value, 100 * 100 + 50);
}

TEST(bgp_ops_test, one_var_intersect_bgp_op_full_subjvar_test){
  TimeControl tc(1000, std::chrono::milliseconds(30000));
  K2TreeConfig config{};
  config.cut_depth = 10;
  config.max_node_count = 128;
  config.treedepth = 32;
  auto k2tree = std::make_unique<K2TreeMixed>(config);
  K2TreeBulkOp bulk_insert(*k2tree);
  for(int i = 100; i < 200; i++){
    for(int j = i*i; j < i*i+100; j++){
      bulk_insert.insert(j , i);
    }
  }

  auto scanner =k2tree->create_band_scanner(100, k2cache::K2TreeScanner::ROW_BAND_TYPE);
  auto op = std::make_unique<OneVarIntersectBGPOp<BGPOp::SUBJECT_VAR>>(
      std::move(scanner), 0, tc);
  std::vector<unsigned long> row_to_fill(1, 100*100 + 50);
  bool done = false;
  int curr = 0;
  unsigned long match_value = 0;
  int valid_count = 0;
  do {
    auto run_result = op->run(row_to_fill);
    done = run_result.scan_done;
//    ASSERT_TRUE(run_result.valid_value) << "failed at curr = " << curr;
    curr++;
    if(run_result.valid_value){
      match_value = row_to_fill[0];
      valid_count++;
    }
  } while (!done);
  ASSERT_EQ(valid_count, 1);
  ASSERT_EQ(match_value, 100 * 100 + 50);
}


TEST(bgp_ops_test, two_var_cross_product_test){
  TimeControl tc(1000, std::chrono::milliseconds(30000));
  K2TreeConfig config{};
  config.cut_depth = 10;
  config.max_node_count = 128;
  config.treedepth = 32;
  auto k2tree = std::make_unique<K2TreeMixed>(config);
  K2TreeBulkOp bulk_insert(*k2tree);
  for(int i = 100; i < 200; i++){
    bulk_insert.insert(i , i);
  }

  auto scanner =k2tree->create_full_scanner();
  auto op = std::make_unique<TwoVarCProductBGPOp>(
      std::move(scanner), 0, 1, tc);
  std::vector<unsigned long> row_to_fill(2, 100*100 + 50);
  bool done = false;
  int curr = 0;
  unsigned long match_value_1 = 0;
  unsigned long match_value_2 = 0;
  int valid_count = 0;
  do {
    auto run_result = op->run(row_to_fill);
    done = run_result.scan_done;
//    ASSERT_TRUE(run_result.valid_value) << "failed at curr = " << curr;
    curr++;
    if(run_result.valid_value){
      match_value_1 = row_to_fill[0];
      match_value_2 = row_to_fill[1];
      ASSERT_EQ(match_value_1, valid_count+100);
      ASSERT_EQ(match_value_2, valid_count+100);
      valid_count++;
    }
  } while (!done);
  ASSERT_EQ(valid_count, 100);
  ASSERT_EQ(match_value_1, 199);
  ASSERT_EQ(match_value_2, 199);
}

