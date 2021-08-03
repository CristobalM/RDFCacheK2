//
// Created by Cristobal Miranda, 2020
//

#include <K2Tree.hpp>
#include <K2TreeBulkOp.hpp>
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

  ASSERT_EQ(debug_validate_block_rec(k2Tree.get_root_block()), 0);
  ASSERT_EQ(debug_validate_block_rec(other_k2tree.get_root_block()), 0);
}

TEST(k2tree_serialization_custom_mixed, test1) {
  K2TreeMixed k2Tree(32, 128);

  int cols = 300;
  int rows = 300;

  std::cout << "inserting" << std::endl;
  K2TreeBulkOp bulk_op(k2Tree);
  for (int i = 0; i < cols; i++) {
    for (int j = 0; j < rows; j++) {
      bulk_op.insert(i, j);
    }
  }

  auto first_points = k2Tree.get_all_points(bulk_op.get_stw());
  std::sort(first_points.begin(), first_points.end(), sort_pair);

  std::ostringstream oss;
  k2Tree.write_to_ostream(oss);

  auto serialized = oss.str();

  std::istringstream iss(serialized);

  K2TreeMixed other_k2tree = K2TreeMixed::read_from_istream(iss);
  K2TreeBulkOp other_bulk_op(other_k2tree);
  auto second_points = other_k2tree.get_all_points(other_bulk_op.get_stw());
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
      ASSERT_TRUE(other_bulk_op.has(i, j))
          << "point (" << i << ", " << j << ") not found";
    }
  }
  for (int i = cols; i < 700; i++) {
    for (int j = 0; j < 700; j++) {
      ASSERT_FALSE(other_bulk_op.has(i, j))
          << "point (" << i << ", " << j << ") exists and shouldn't";
    }
  }
  for (int i = 0; i < 700; i++) {
    for (int j = rows; j < 700; j++) {
      ASSERT_FALSE(other_bulk_op.has(i, j))
          << "point (" << i << ", " << j << ") exists and shouldn't";
    }
  }

  ASSERT_FALSE(other_bulk_op.has(1 << 30, 1 << 30))
      << "point (" << 1 << 30 << ", " << 1 << 30 << ") exists and shouldn't";

  ASSERT_TRUE(k2Tree.has_valid_structure(bulk_op.get_stw()));
  ASSERT_TRUE(other_k2tree.has_valid_structure(other_bulk_op.get_stw()));
}

TEST(k2tree_serialization_custom_mixed, sip_band_works_on_deserialized_1) {
  K2TreeMixed k2tree(32, 256, 10);

  int cols = 300;
  int rows = 300;

  K2TreeBulkOp bulk_op(k2tree);

  std::cout << "inserting" << std::endl;
  for (int i = 0; i < cols; i++) {
    for (int j = 0; j < rows; j++) {
      bulk_op.insert(i, j);
    }
  }

  auto first_points = k2tree.get_all_points();
  std::sort(first_points.begin(), first_points.end(), sort_pair);

  std::ostringstream oss;
  k2tree.write_to_ostream(oss);

  auto serialized = oss.str();

  std::istringstream iss(serialized);

  K2TreeMixed other_k2tree = K2TreeMixed::read_from_istream(iss);

  std::vector<const K2TreeMixed *> trees = {&other_k2tree};

  for (int i = 0; i < cols; i++) {

    sip_ipoint join_coordinate;
    join_coordinate.coord = i;
    join_coordinate.coord_type = COLUMN_COORD;

    std::vector<sip_ipoint> join_coordinates = {join_coordinate};
    auto join_result = K2TreeMixed::sip_join_k2trees(trees, join_coordinates);

    for (int j = 0; j < (int)join_result.size(); j++) {
      ASSERT_EQ(join_result[j], j) << "(cols) Failed at " << i << ", " << j;
    }
  }

  for (int i = 0; i < rows; i++) {

    sip_ipoint join_coordinate;
    join_coordinate.coord = i;
    join_coordinate.coord_type = ROW_COORD;

    std::vector<sip_ipoint> join_coordinates = {join_coordinate};
    auto join_result = K2TreeMixed::sip_join_k2trees(trees, join_coordinates);

    for (int j = 0; j < (int)join_result.size(); j++) {
      ASSERT_EQ(join_result[j], j) << "(rows) Failed at " << i << ", " << j;
    }
  }

  ASSERT_TRUE(k2tree.same_as(other_k2tree));
  ASSERT_TRUE(other_k2tree.same_as(k2tree));

  ASSERT_TRUE(k2tree.has_valid_structure(bulk_op.get_stw()));
  ASSERT_TRUE(other_k2tree.has_valid_structure());
}
