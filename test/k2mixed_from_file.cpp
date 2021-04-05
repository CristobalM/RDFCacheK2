/*
MIT License

Copyright (c) 2020 Cristobal Miranda T.

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
*/
#include <gtest/gtest.h>

#include <K2TreeMixed.hpp>
#include <algorithm>
#include <fstream>
#include <iostream>
#include <sstream>
#include <string>
#include <utility>
#include <vector>

class K2MixedFromFileFixture : public ::testing::Test {

protected:
  virtual void SetUp() override {}
  virtual void TearDown() override {}
  virtual void TestBody() override {}

public:
  K2MixedFromFileFixture() {}

  static std::unique_ptr<K2TreeMixed> k2tree;

  static void SetUpTestCase() {
    std::ifstream ifs("k2tree_case_1.txt", std::ios::in);

    TREE_DEPTH_T treedepth = 32;
    TREE_DEPTH_T cutdepth = 10;
    k2tree = std::make_unique<K2TreeMixed>(treedepth, 255, cutdepth);

    std::string line;
    std::string number_string;
    while (std::getline(ifs, line)) {
      std::stringstream ss(line);
      int i = 0;
      unsigned long numbers[2];
      while (std::getline(ss, number_string, ',')) {
        numbers[i++] = std::stoul(number_string);
      }
      k2tree->insert(numbers[0], numbers[1]);
    }
  }

  static void TearDownTestCase() { k2tree = nullptr; }
};

std::unique_ptr<K2TreeMixed> K2MixedFromFileFixture::k2tree = nullptr;

struct coords_sets {
  std::set<ulong> &cols;
  std::set<ulong> &rows;
  coords_sets(std::set<ulong> &cols, std::set<ulong> &rows)
      : cols(cols), rows(rows) {}
};

TEST_F(K2MixedFromFileFixture, single_sip_band_by_band) {

  std::set<ulong> cols;
  std::set<ulong> rows;
  coords_sets sets(cols, rows);

  k2tree->scan_points(
      [](unsigned long col, unsigned long row, void *report_state) {
        auto &sets = *reinterpret_cast<coords_sets *>(report_state);
        sets.cols.insert(col);
        sets.rows.insert(row);
      },
      &sets);

  std::vector<const K2TreeMixed *> trees = {k2tree.get()};
  for (auto it = cols.begin(); it != cols.end(); it++) {

    sip_ipoint join_coordinate;
    join_coordinate.coord = *it;
    join_coordinate.coord_type = COLUMN_COORD;

    std::vector<sip_ipoint> join_coordinates = {join_coordinate};
    auto join_result = K2TreeMixed::sip_join_k2trees(trees, join_coordinates);

    auto coords_join =
        std::set<unsigned long>(join_result.begin(), join_result.end());

    std::set<ulong> coords_report;

    k2tree->traverse_column(
        *it,
        [](unsigned long, unsigned long row, void *report_state) {
          auto &data = *reinterpret_cast<std::set<ulong> *>(report_state);
          data.insert(row);
        },
        &coords_report);

    ASSERT_EQ(coords_join, coords_report) << "Failed at column " << *it;
  }

  for (auto it = rows.begin(); it != rows.end(); it++) {

    sip_ipoint join_coordinate;
    join_coordinate.coord = *it;
    join_coordinate.coord_type = ROW_COORD;

    std::vector<sip_ipoint> join_coordinates = {join_coordinate};
    auto join_result = K2TreeMixed::sip_join_k2trees(trees, join_coordinates);

    auto coords_join =
        std::set<unsigned long>(join_result.begin(), join_result.end());

    std::set<ulong> coords_report;

    k2tree->traverse_row(
        *it,
        [](unsigned long col, unsigned long, void *report_state) {
          auto &data = *reinterpret_cast<std::set<ulong> *>(report_state);
          data.insert(col);
        },
        &coords_report);

    ASSERT_EQ(coords_join, coords_report) << "Failed at row " << *it;
  }

  ASSERT_TRUE(k2tree->has_valid_structure());
}
