//
// Created by Cristobal Miranda, 2020
//

#include <K2Tree.hpp>
#include <K2TreeMixed.hpp>
#include <algorithm>
#include <gtest/gtest.h>
#include <random>
#include <sstream>

using ul = unsigned long;
bool sort_pair(const std::pair<ul, ul> &lhs, const std::pair<ul, ul> &rhs) {
  if (lhs.first != rhs.first)
    return lhs.first < rhs.first;
  return lhs.second < rhs.second;
}

TEST(k2tree_serialization_custom, test1) {
  K2Tree k2Tree(32, 128);

  int cols = 300;
  int rows = 300;

  std::cout << "inserting" << std::endl;
  for (int i = 0; i < cols; i++) {
    for (int j = 0; j < rows; j++) {
      k2Tree.insert(i, j);
    }
  }

  auto first_points = k2Tree.get_all_points();
  std::sort(first_points.begin(), first_points.end(), sort_pair);

  std::ostringstream oss;
  k2Tree.write_to_ostream(oss);

  auto serialized = oss.str();

  std::istringstream iss(serialized);

  K2Tree other_k2tree = K2Tree::read_from_istream(iss);

  auto second_points = other_k2tree.get_all_points();
  std::sort(second_points.begin(), second_points.end(), sort_pair);

  ASSERT_EQ(first_points.size(), second_points.size())
      << "k2trees have different size";

  for (size_t i = 0; i < first_points.size(); i++) {
    ASSERT_EQ(first_points[i], second_points[i])
        << "k2trees points differ at i = " << i;
  }

  for (int i = 0; i < cols; i++) {
    for (int j = 0; j < rows; j++) {
      ASSERT_TRUE(other_k2tree.has(i, j))
          << "point (" << i << ", " << j << ") not found";
    }
  }
  for (int i = cols; i < 700; i++) {
    for (int j = 0; j < 700; j++) {
      ASSERT_FALSE(other_k2tree.has(i, j))
          << "point (" << i << ", " << j << ") exists and shouldn't";
    }
  }
  for (int i = 0; i < 700; i++) {
    for (int j = rows; j < 700; j++) {
      ASSERT_FALSE(other_k2tree.has(i, j))
          << "point (" << i << ", " << j << ") exists and shouldn't";
    }
  }

  ASSERT_FALSE(other_k2tree.has(1 << 30, 1 << 30))
      << "point (" << 1 << 30 << ", " << 1 << 30 << ") exists and shouldn't";
}

TEST(k2tree_serialization_custom_mixed, test1) {
  K2TreeMixed k2Tree(32, 128);

  int cols = 300;
  int rows = 300;

  std::cout << "inserting" << std::endl;
  for (int i = 0; i < cols; i++) {
    for (int j = 0; j < rows; j++) {
      k2Tree.insert(i, j);
    }
  }

  auto first_points = k2Tree.get_all_points();
  std::sort(first_points.begin(), first_points.end(), sort_pair);

  std::ostringstream oss;
  k2Tree.write_to_ostream(oss);

  auto serialized = oss.str();

  std::istringstream iss(serialized);

  K2TreeMixed other_k2tree = K2TreeMixed::read_from_istream(iss);

  auto second_points = other_k2tree.get_all_points();
  std::sort(second_points.begin(), second_points.end(), sort_pair);

  ASSERT_EQ(first_points.size(), cols * rows) << "Invalid size";

  ASSERT_EQ(first_points.size(), second_points.size())
      << "k2trees have different size";

  for (size_t i = 0; i < first_points.size(); i++) {
    ASSERT_EQ(first_points[i], second_points[i])
        << "k2trees points differ at i = " << i;
  }

  for (int i = 0; i < cols; i++) {
    for (int j = 0; j < rows; j++) {
      ASSERT_TRUE(other_k2tree.has(i, j))
          << "point (" << i << ", " << j << ") not found";
    }
  }
  for (int i = cols; i < 700; i++) {
    for (int j = 0; j < 700; j++) {
      ASSERT_FALSE(other_k2tree.has(i, j))
          << "point (" << i << ", " << j << ") exists and shouldn't";
    }
  }
  for (int i = 0; i < 700; i++) {
    for (int j = rows; j < 700; j++) {
      ASSERT_FALSE(other_k2tree.has(i, j))
          << "point (" << i << ", " << j << ") exists and shouldn't";
    }
  }

  ASSERT_FALSE(other_k2tree.has(1 << 30, 1 << 30))
      << "point (" << 1 << 30 << ", " << 1 << 30 << ") exists and shouldn't";
}
