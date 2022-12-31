//
// Created by Cristobal Miranda, 2020
//

#include <gtest/gtest.h>
#include <set>

#include "k2tree/K2TreeMixed.hpp"
#include <google/protobuf/stubs/common.h>
using namespace k2cache;

using pairs_set = std::set<std::pair<uint64_t, uint64_t>>;

struct DataOne {
  int a;
};

TEST(k2tree_tests, scan_test_1) {
  K2TreeMixed k2Tree(32, 256);

  for (int i = 0; i < 100; i++) {
    for (int j = 0; j < 100; j++) {
      k2Tree.insert(i, j);
    }
  }

  pairs_set points;

  k2Tree.scan_points(
      [](uint64_t column, uint64_t row, void *_data) {
        auto &points = *reinterpret_cast<pairs_set *>(_data);
        points.insert({column, row});
      },
      &points);

  ASSERT_EQ(points.size(), 10000)
      << "Points amount retrieved is different than expected in full scan";

  points.clear();

  k2Tree.traverse_row(
      0,
      [](uint64_t column, uint64_t row, void *_data) {
        auto &points = *reinterpret_cast<pairs_set *>(_data);
        points.insert({column, row});
      },
      &points);

  ASSERT_EQ(points.size(), 100)
      << "Points amount retrieved is different than expected in row scan";

  points.clear();

  k2Tree.traverse_column(
      0,
      [](uint64_t column, uint64_t row, void *_data) {
        auto &points = *reinterpret_cast<pairs_set *>(_data);
        points.insert({column, row});
      },
      &points);
  ASSERT_EQ(points.size(), 100)
      << "Points amount retrieved is different than expected in column scan";
}

int main(int argc, char **argv) {
  ::testing::InitGoogleTest(&argc, argv);
  int result = RUN_ALL_TESTS();
  google::protobuf::ShutdownProtobufLibrary();
  return result;
}
