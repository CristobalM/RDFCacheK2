#include <gtest/gtest.h>

#include <K2TreeMixed.hpp>
#include <set>
#include <sstream>
#include <utility>

TEST(k2tree_mixed_tests, can_insert) {
  K2TreeMixed tree(10);
  tree.insert(3, 3);
  tree.insert(5, 3);
  ASSERT_TRUE(tree.has(3, 3));
  ASSERT_TRUE(tree.has(5, 3));
  ASSERT_FALSE(tree.has(5, 2));
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
}

TEST(k2tree_mixed_test, can_sip_join) {
  std::set<std::pair<unsigned long, unsigned long>> points_to_insert = {
      {0, 3}, {2, 1}, {123, 321}, {44, 41}, {33, 21}, {6, 9}, {5, 3}, {100, 3}};

  unsigned long treedepth = 10;
  K2TreeMixed tree(treedepth, 255, 5);
  for (auto point : points_to_insert) {
    tree.insert(point.first, point.second);
  }
  std::vector<K2TreeMixed *> trees_to_join = {&tree};
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
}
