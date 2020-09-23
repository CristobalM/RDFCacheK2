//
// Created by Cristobal Miranda, 2020
//

#include "ResultTable.hpp"
#include <gtest/gtest.h>
#include <iostream>
#include <random>

TEST(ResultTableTest, test1) {
  std::vector<unsigned long> v1 = {1, 2, 3, 4, 5, 6, 7};
  std::vector<unsigned long> v2 = {1, 2, 6, 7};
  ResultTable result_table(1, std::move(v1));
  ResultTable result_table2(1, std::move(v2));
  result_table.join_with(0, 0, std::move(result_table2));
  auto &data = result_table.get_data();

  for (auto it = data.begin(); it != data.end(); it++) {
    auto &v = *it;
    std::cout << v[0] << std::endl;
  }
}