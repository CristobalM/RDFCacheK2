//
// Created by Cristobal Miranda, 2020
//

#include <gtest/gtest.h>
#include <iostream>
#include <random>

#include <K2Tree.hpp>

struct DataOne {
  int a;
};

TEST(k2tree_tests, scan_test_1) {
  K2Tree k2Tree(32, 1024);

  for (int i = 0; i < 100; i++) {
    for (int j = 0; j < 100; j++) {
      k2Tree.insert(i, j);
    }
  }

  DataOne data = {1};

  k2Tree.scan_points(
      [](unsigned long column, unsigned long row, void *_data) {
        auto &data = *reinterpret_cast<DataOne *>(_data);
        std::cout << "on ( " << data.a << ")  (" << column << ", " << row << ")"
                  << std::endl;
      },
      &data);

  k2Tree.traverse_row(
      0,
      [](unsigned long column, unsigned long row, void *_data) {
        auto &data = *reinterpret_cast<DataOne *>(_data);
        std::cout << "with row 0 on ( " << data.a << ")  (" << column << ", "
                  << row << ")" << std::endl;
      },
      &data);

  k2Tree.traverse_column(
      0,
      [](unsigned long column, unsigned long row, void *_data) {
        auto &data = *reinterpret_cast<DataOne *>(_data);
        std::cout << "with column 0 on ( " << data.a << ")  (" << column << ", "
                  << row << ")" << std::endl;
      },
      &data);
}
