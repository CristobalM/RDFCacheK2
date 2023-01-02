#include <gtest/gtest.h>

#include "k2tree/K2TreeBulkOp.hpp"
#include "k2tree/K2TreeMixed.hpp"
#include <chrono>
#include <google/protobuf/message_lite.h>
#include <set>
#include <utility>

using namespace std::chrono_literals;
using namespace k2cache;

TEST(k2tree_mixed_tests, can_insert) {
  K2TreeMixed tree(10);
  K2TreeBulkOp bulk_op(tree);
  bulk_op.insert(3, 3);
  bulk_op.insert(5, 3);
  ASSERT_TRUE(bulk_op.has(3, 3));
  ASSERT_TRUE(bulk_op.has(5, 3));
  ASSERT_FALSE(bulk_op.has(5, 2));
  ASSERT_TRUE(tree.has_valid_structure(bulk_op.get_stw()));
}

TEST(k2tree_mixed_tests, same_insertion_doesnt_increase_size) {
  K2TreeMixed tree(10);
  K2TreeBulkOp bulk_op(tree);
  for (int i = 0; i < 100; i++)
    bulk_op.insert(3, 3);
  ASSERT_EQ(tree.size(), 1);
  ASSERT_TRUE(tree.has_valid_structure(bulk_op.get_stw()));
}

TEST(k2tree_mixed_test, exhaustive_validation) {
  std::set<std::pair<uint64_t, uint64_t>> points_to_insert = {
      {0, 3}, {2, 1}, {123, 321}, {44, 41}, {33, 21}, {6, 9}};

  uint64_t treedepth = 10;
  uint64_t side = 1 << treedepth;
  K2TreeMixed tree(10, 255, 5);
  K2TreeBulkOp bulk_op(tree);
  for (auto point : points_to_insert) {
    bulk_op.insert(point.first, point.second);
  }
  for (auto point : points_to_insert) {
    ASSERT_TRUE(bulk_op.has(point.first, point.second));
  }

  for (uint64_t col = 0; col < side; col++) {
    for (uint64_t row = 0; row < side; row++) {
      auto pair = std::make_pair(col, row);
      if (points_to_insert.find(pair) == points_to_insert.end()) {
        ASSERT_FALSE(bulk_op.has(col, row));
      }
    }
  }
  ASSERT_TRUE(tree.has_valid_structure(bulk_op.get_stw()));
}

TEST(k2tree_mixed_test, can_serialize) {
  std::set<std::pair<uint64_t, uint64_t>> points_to_insert = {
      {0, 3}, {2, 1}, {123, 321}, {44, 41}, {33, 21}, {6, 9}};

  uint64_t treedepth = 10;
  uint64_t side = 1 << treedepth;
  K2TreeMixed tree(10, 255, 5);
  K2TreeBulkOp bulk_op(tree);
  for (auto point : points_to_insert) {
    bulk_op.insert(point.first, point.second);
  }

  std::stringstream ss;
  tree.write_to_ostream(ss);

  ss.seekg(0);

  K2TreeMixed deserialized = K2TreeMixed::read_from_istream(ss);
  K2TreeBulkOp deserialized_bulk_op(deserialized);
  for (auto point : points_to_insert) {
    ASSERT_TRUE(deserialized_bulk_op.has(point.first, point.second))
        << "Point (" << point.first << ", " << point.second << ") not found";
  }

  for (uint64_t col = 0; col < side; col++) {
    for (uint64_t row = 0; row < side; row++) {
      auto pair = std::make_pair(col, row);
      if (points_to_insert.find(pair) == points_to_insert.end()) {
        ASSERT_FALSE(deserialized_bulk_op.has(col, row))
            << "Point (" << pair.first << ", " << pair.second
            << ") was found, but shouldn't exist";
      }
    }
  }

  ASSERT_TRUE(tree.has_valid_structure(bulk_op.get_stw()));
  ASSERT_TRUE(deserialized.has_valid_structure(deserialized_bulk_op.get_stw()));
}

TEST(k2tree_mixed_test, can_report_column) {
  uint64_t treedepth = 10;
  K2TreeMixed tree(treedepth, 255, 5);
  K2TreeBulkOp bulk_op(tree);
  for (int i = 1; i < 100; i++) {
    bulk_op.insert(1, i);
  }

  std::vector<uint64_t> retrieved;
  tree.traverse_column(
      1,
      [](uint64_t, uint64_t row, void *report_state) {
        reinterpret_cast<std::vector<uint64_t> *>(report_state)
            ->push_back(row);
      },
      &retrieved, bulk_op.get_stw());

  for (int i = 0; i < 99; i++) {
    ASSERT_EQ(retrieved[i], i + 1);
  }
  ASSERT_TRUE(tree.has_valid_structure(bulk_op.get_stw()));
}

TEST(k2tree_mixed_test, can_report_row) {
  uint64_t treedepth = 10;
  K2TreeMixed tree(treedepth, 255, 5);
  K2TreeBulkOp bulk_op(tree);
  for (int i = 1; i < 100; i++) {
    bulk_op.insert(i, 1);
  }

  std::vector<uint64_t> retrieved;
  tree.traverse_row(
      1,
      [](uint64_t col, uint64_t, void *report_state) {
        reinterpret_cast<std::vector<uint64_t> *>(report_state)
            ->push_back(col);
      },
      &retrieved, bulk_op.get_stw());

  for (int i = 0; i < 99; i++) {
    ASSERT_EQ(retrieved[i], i + 1);
  }
  ASSERT_TRUE(tree.has_valid_structure(bulk_op.get_stw()));
}

TEST(k2tree_mixed_test, can_report_row_2) {
  uint64_t treedepth = 10;
  K2TreeMixed tree(treedepth, 255, 5);
  K2TreeBulkOp bulk_op(tree);
  for (int i = 1; i < 100; i++) {
    bulk_op.insert(i, 1);
  }

  std::vector<uint64_t> retrieved;
  tree.traverse_row(
      1,
      [](uint64_t col, uint64_t, void *report_state) {
        reinterpret_cast<std::vector<uint64_t> *>(report_state)
            ->push_back(col);
      },
      &retrieved, bulk_op.get_stw());

  std::vector<const K2TreeMixed *> trees;

  trees.push_back(&tree);

  ASSERT_TRUE(tree.has_valid_structure());
}

TEST(k2tree_mixed_test, can_scan_full_lazy_with_virtual_scanner) {
  uint64_t treedepth = 10;
  K2TreeMixed tree(treedepth, 255, 5);
  K2TreeBulkOp bulk_op(tree);
  for (int i = 1; i < 100; i++) {
    bulk_op.insert(i, 1);
  }

  uint64_t i = 1;
  auto scanner = tree.create_full_scanner();

  while (scanner->has_next()) {
    auto curr = scanner->next();
    ASSERT_EQ(curr.first, i++);
  }
  ASSERT_EQ(i, 100);
}

TEST(k2tree_mixed_test, can_scan_band_lazy_with_virtual_scanner) {
  uint64_t treedepth = 10;
  K2TreeMixed tree(treedepth, 255, 5);
  K2TreeBulkOp bulk_op(tree);
  for (int i = 1; i < 100; i++) {
    bulk_op.insert(i, 1);
  }

  uint64_t i = 1;
  auto scanner =
      tree.create_band_scanner(1, K2TreeScanner::BandType::ROW_BAND_TYPE);

  while (scanner->has_next()) {
    auto curr = scanner->next();
    ASSERT_EQ(curr.first, i++);
  }
  ASSERT_EQ(i, 100);
}

TEST(k2tree_mixed_test, check_if_64_depth_works_1) {
  uint64_t treedepth = 64;
  K2TreeMixed tree(treedepth, 256, 10);
  K2TreeBulkOp bulk_op(tree);
  constexpr size_t values_sz = 10;
  std::set<std::pair<uint64_t, uint64_t>> values;
  for (size_t i = 1; i <= values_sz; i++) {
    // values.insert({(1UL << 38) + i, (1UL << 38) + i});
    // values.insert({(1UL << 38UL) + i, (1UL << 38UL) + i});
    auto exp = 63UL;
    auto value = (1UL << exp) + i;
    values.insert({value, value});
  }
  for (const auto &vp : values) {
    bulk_op.insert(vp.first, vp.second);
  }
  K2QStateWrapper kst(treedepth, 10, 256);
  tree.scan_points(
      [](uint64_t col, uint64_t row, void *rs) {
        auto &values = *reinterpret_cast<
            std::set<std::pair<uint64_t, uint64_t>> *>(rs);
        // std::cout << col << ", " << row << "\n";
        auto found = values.find({col, row}) != values.end();
        ASSERT_TRUE(found) << "failed with pair (" << col << ", " << row << ")";
      },
      &values, kst);

  auto scanner = tree.create_full_scanner();
  size_t i = 0;
  while (scanner->has_next()) {
    auto next_pair = scanner->next();
    auto found = values.find(next_pair) != values.end();
    ASSERT_TRUE(found) << "failed with pair (" << next_pair.first << ", "
                       << next_pair.second << ") i = " << i;
    i++;
  }

  for (const auto &vp : values) {
    ASSERT_TRUE(tree.has(vp.first, vp.second));
  }
}

TEST(k2tree_mixed_test, can_copy_full_tree) {
  uint64_t treedepth = 64;
  K2TreeMixed tree(treedepth, 256, 10);
  K2TreeBulkOp bulk_op(tree);
  constexpr size_t values_sz = 10;
  std::set<std::pair<uint64_t, uint64_t>> values;
  for (size_t i = 1; i <= values_sz; i++) {
    auto exp = 63UL;
    auto value = (1UL << exp) + i;
    values.insert({value, value});
  }
  for (const auto &vp : values) {
    bulk_op.insert(vp.first, vp.second);
  }
  auto copied_tree = K2TreeMixed(tree);
  ASSERT_TRUE(copied_tree.identical_structure_as(tree));
  ASSERT_FALSE(copied_tree.shares_any_reference_to(tree));
}

int main(int argc, char **argv) {
  ::testing::InitGoogleTest(&argc, argv);
  int result = RUN_ALL_TESTS();
  google::protobuf::ShutdownProtobufLibrary();
  return result;
}
