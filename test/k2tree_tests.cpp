//
// Created by Cristobal Miranda, 2020
//

#include <gtest/gtest.h>
#include <iostream>
#include <random>
#include <set>

#include <K2Tree.hpp>

using pairs_set = std::set<std::pair<unsigned long, unsigned long>>;

struct DataOne {
  int a;
};

TEST(k2tree_tests, scan_test_1) {
  K2Tree k2Tree(32, 128);

  for (int i = 0; i < 100; i++) {
    for (int j = 0; j < 100; j++) {
      k2Tree.insert(i, j);
    }
  }

  pairs_set points;

  k2Tree.scan_points(
      [](unsigned long column, unsigned long row, void *_data) {
        auto &points = *reinterpret_cast<pairs_set *>(_data);
        points.insert({column, row});
      },
      &points);

  ASSERT_EQ(points.size(), 10000)
      << "Points amount retrieved is different than expected in full scan";

  points.clear();

  k2Tree.traverse_row(
      0,
      [](unsigned long column, unsigned long row, void *_data) {
        auto &points = *reinterpret_cast<pairs_set *>(_data);
        points.insert({column, row});
      },
      &points);

  ASSERT_EQ(points.size(), 100)
      << "Points amount retrieved is different than expected in row scan";

  points.clear();

  k2Tree.traverse_column(
      0,
      [](unsigned long column, unsigned long row, void *_data) {
        auto &points = *reinterpret_cast<pairs_set *>(_data);
        points.insert({column, row});
      },
      &points);
  ASSERT_EQ(points.size(), 100)
      << "Points amount retrieved is different than expected in column scan";
}

TEST(k2tree_tests, scan_test_create_table) {
  K2Tree k2Tree(32, 1024);

  for (int i = 0; i < 100; i++) {
    for (int j = 0; j < 100; j++) {
      k2Tree.insert(i, j);
    }
  }

  K2Tree k2Tree_2(32, 1024);

  for (int i = 99; i < 1000; i++) {
    k2Tree_2.insert(i, 3);
  }

  auto table_col_0 = k2Tree.column_as_table(0);
  auto table_row_3 = k2Tree_2.row_as_table(3);

  int counter = 0;
  for (auto it = table_col_0.get_data().begin();
       it != table_col_0.get_data().end(); it++) {
    ASSERT_EQ((*it)[0], counter) << "Table 0 fails at counter = " << counter;
    counter++;
  }

  counter = 99;
  for (auto it = table_row_3.get_data().begin();
       it != table_row_3.get_data().end(); it++) {
    ASSERT_EQ((*it)[0], counter) << "Table 3 fails at counter = " << counter;
    counter++;
  }

  table_col_0.left_inner_join_with(0, 0, table_row_3);

  auto &joined_data = table_col_0.get_data();

  ASSERT_EQ(joined_data.size(), 1) << "Joined data size different than 1";

  auto it = joined_data.begin();

  ASSERT_EQ((*it)[0], 99) << "Join result different than 99";
}