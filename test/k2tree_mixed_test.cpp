#include <gtest/gtest.h>

#include <K2TreeMixed.hpp>
#include <chrono>
#include <set>
#include <sstream>
#include <utility>

using namespace std::chrono_literals;
TimeControl time_control(1e12, 100min);

TEST(k2tree_mixed_tests, single_band_sip_1) {
  const uint32_t tree_depth = 10;
  const uint32_t matrix_side = 1 << tree_depth;

  K2TreeMixed tree(tree_depth, 256, 5);
  for (unsigned long i = 0; i < matrix_side; i++) {
    tree.insert(i, 5);
  }
  for (unsigned long i = 0; i < matrix_side; i++) {
    std::vector<const K2TreeMixed *> trees = {&tree};
    sip_ipoint join_coordinate;
    join_coordinate.coord = i;
    join_coordinate.coord_type = COLUMN_COORD;

    std::vector<sip_ipoint> join_coordinates = {join_coordinate};
    auto join_result = K2TreeMixed::sip_join_k2trees(trees, join_coordinates);
    ASSERT_EQ(join_result.size(), 1);
    ASSERT_EQ(join_result[0], 5);
  }

  ASSERT_TRUE(tree.has_valid_structure());
}

TEST(k2tree_mixed_tests, can_insert) {
  K2TreeMixed tree(10);
  tree.insert(3, 3);
  tree.insert(5, 3);
  ASSERT_TRUE(tree.has(3, 3));
  ASSERT_TRUE(tree.has(5, 3));
  ASSERT_FALSE(tree.has(5, 2));
  ASSERT_TRUE(tree.has_valid_structure());
}

TEST(k2tree_mixed_tests, same_insertion_doesnt_increase_size) {
  K2TreeMixed tree(10);
  for (int i = 0; i < 100; i++)
    tree.insert(3, 3);
  ASSERT_EQ(tree.size(), 1);
  ASSERT_TRUE(tree.has_valid_structure());
}

TEST(k2tree_mixed_test, exhaustive_validation) {
  std::set<std::pair<unsigned long, unsigned long>> points_to_insert = {
      {0, 3}, {2, 1}, {123, 321}, {44, 41}, {33, 21}, {6, 9}};

  unsigned long treedepth = 10;
  unsigned long side = 1 << treedepth;
  K2TreeMixed tree(10, 255, 5);
  for (auto point : points_to_insert) {
    tree.insert(point.first, point.second);
  }
  for (auto point : points_to_insert) {
    ASSERT_TRUE(tree.has(point.first, point.second));
  }

  for (unsigned long col = 0; col < side; col++) {
    for (unsigned long row = 0; row < side; row++) {
      auto pair = std::make_pair(col, row);
      if (points_to_insert.find(pair) == points_to_insert.end()) {
        ASSERT_FALSE(tree.has(col, row));
      }
    }
  }
  ASSERT_TRUE(tree.has_valid_structure());
}

TEST(k2tree_mixed_test, can_sip_join) {
  std::set<std::pair<unsigned long, unsigned long>> points_to_insert = {
      {0, 3}, {2, 1}, {123, 321}, {44, 41}, {33, 21}, {6, 9}, {5, 3}, {100, 3}};

  unsigned long treedepth = 10;
  K2TreeMixed tree(treedepth, 255, 5);
  for (auto point : points_to_insert) {
    tree.insert(point.first, point.second);
  }
  std::vector<const K2TreeMixed *> trees_to_join = {&tree};
  std::vector<struct sip_ipoint> coords;
  struct sip_ipoint first;
  first.coord = 3;
  first.coord_type = ROW_COORD;
  coords.push_back(first);
  auto result = K2TreeMixed::sip_join_k2trees(trees_to_join, coords);
  ASSERT_EQ(result.size(), 3);
  ASSERT_EQ(result[0], 0);
  ASSERT_EQ(result[1], 5);
  ASSERT_EQ(result[2], 100);
  ASSERT_TRUE(tree.has_valid_structure());
}

TEST(k2tree_mixed_test, can_serialize) {
  std::set<std::pair<unsigned long, unsigned long>> points_to_insert = {
      {0, 3}, {2, 1}, {123, 321}, {44, 41}, {33, 21}, {6, 9}};

  unsigned long treedepth = 10;
  unsigned long side = 1 << treedepth;
  K2TreeMixed tree(10, 255, 5);
  for (auto point : points_to_insert) {
    tree.insert(point.first, point.second);
  }

  std::stringstream ss;
  tree.write_to_ostream(ss);

  ss.seekg(0);

  K2TreeMixed deserialized = K2TreeMixed::read_from_istream(ss);

  for (auto point : points_to_insert) {
    ASSERT_TRUE(deserialized.has(point.first, point.second))
        << "Point (" << point.first << ", " << point.second << ") not found";
  }

  for (unsigned long col = 0; col < side; col++) {
    for (unsigned long row = 0; row < side; row++) {
      auto pair = std::make_pair(col, row);
      if (points_to_insert.find(pair) == points_to_insert.end()) {
        ASSERT_FALSE(deserialized.has(col, row))
            << "Point (" << pair.first << ", " << pair.second
            << ") was found, but shouldn't exist";
      }
    }
  }

  ASSERT_TRUE(tree.has_valid_structure());
  ASSERT_TRUE(deserialized.has_valid_structure());
}

TEST(k2tree_mixed_test, can_report_column) {
  unsigned long treedepth = 10;
  K2TreeMixed tree(treedepth, 255, 5);

  for (int i = 1; i < 100; i++) {
    tree.insert(1, i);
  }

  std::vector<unsigned long> retrieved;
  tree.traverse_column(
      1,
      [](unsigned long, unsigned long row, void *report_state) {
        reinterpret_cast<std::vector<unsigned long> *>(report_state)
            ->push_back(row);
      },
      &retrieved);

  for (int i = 0; i < 99; i++) {
    ASSERT_EQ(retrieved[i], i + 1);
  }
  ASSERT_TRUE(tree.has_valid_structure());
}

TEST(k2tree_mixed_test, can_report_row) {
  unsigned long treedepth = 10;
  K2TreeMixed tree(treedepth, 255, 5);

  for (int i = 1; i < 100; i++) {
    tree.insert(i, 1);
  }

  std::vector<unsigned long> retrieved;
  tree.traverse_row(
      1,
      [](unsigned long col, unsigned long, void *report_state) {
        reinterpret_cast<std::vector<unsigned long> *>(report_state)
            ->push_back(col);
      },
      &retrieved);

  for (int i = 0; i < 99; i++) {
    ASSERT_EQ(retrieved[i], i + 1);
  }
  ASSERT_TRUE(tree.has_valid_structure());
}

TEST(k2tree_mixed_test, can_report_row_and_sip) {
  unsigned long treedepth = 10;
  K2TreeMixed tree(treedepth, 255, 5);

  for (int i = 1; i < 100; i++) {
    tree.insert(i, 1);
  }

  std::vector<unsigned long> retrieved;
  tree.traverse_row(
      1,
      [](unsigned long col, unsigned long, void *report_state) {
        reinterpret_cast<std::vector<unsigned long> *>(report_state)
            ->push_back(col);
      },
      &retrieved);

  std::vector<unsigned long> retrieved_sip;

  std::vector<const K2TreeMixed *> trees;

  trees.push_back(&tree);
  std::vector<struct sip_ipoint> join_coords;

  struct sip_ipoint coord;
  coord.coord = 1;
  coord.coord_type = coord_t::ROW_COORD;

  join_coords.push_back(coord);

  auto result_sip = K2TreeMixed::sip_join_k2trees(trees, join_coords);
  ASSERT_EQ(result_sip.size(), retrieved.size());

  for (int i = 0; i < 99; i++) {
    ASSERT_EQ(retrieved[i], result_sip[i]);
  }

  ASSERT_TRUE(tree.has_valid_structure());
}

TEST(k2tree_mixed_test, can_scan_full_lazy_with_virtual_scanner) {
  unsigned long treedepth = 10;
  K2TreeMixed tree(treedepth, 255, 5);

  for (int i = 1; i < 100; i++) {
    tree.insert(i, 1);
  }

  unsigned long i = 1;
  auto scanner = tree.create_full_scanner(time_control);

  while (scanner->has_next()) {
    auto curr = scanner->next();
    ASSERT_EQ(curr.first, i++);
  }
  ASSERT_EQ(i, 100);
}

TEST(k2tree_mixed_test, can_scan_band_lazy_with_virtual_scanner) {
  unsigned long treedepth = 10;
  K2TreeMixed tree(treedepth, 255, 5);

  for (int i = 1; i < 100; i++) {
    tree.insert(i, 1);
  }

  unsigned long i = 1;
  auto scanner = tree.create_band_scanner(
      1, K2TreeScanner::BandType::ROW_BAND_TYPE, time_control);

  while (scanner->has_next()) {
    auto curr = scanner->next();
    ASSERT_EQ(curr.first, i++);
  }
  ASSERT_EQ(i, 100);
}
