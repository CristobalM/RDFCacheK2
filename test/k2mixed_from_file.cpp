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

#include "k2tree/K2TreeBulkOp.hpp"
#include "k2tree/K2TreeMixed.hpp"
#include <algorithm>
#include <fstream>
#include <google/protobuf/message_lite.h>
#include <iostream>
#include <set>
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
    K2TreeBulkOp bulk_op(*k2tree);
    while (std::getline(ifs, line)) {
      std::stringstream ss(line);
      int i = 0;
      unsigned long numbers[2];
      while (std::getline(ss, number_string, ',')) {
        numbers[i++] = std::stoul(number_string);
      }
      bulk_op.insert(numbers[0], numbers[1]);
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

int main(int argc, char **argv) {
  ::testing::InitGoogleTest(&argc, argv);
  int result = RUN_ALL_TESTS();
  google::protobuf::ShutdownProtobufLibrary();
  return result;
}
