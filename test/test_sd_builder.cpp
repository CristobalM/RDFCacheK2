//
// Created by Cristobal Miranda, 2020
//

#include <gtest/gtest.h>

#include <SDBuilder.hpp>
#include <unordered_set>

TEST(sd_builder_suite, test_1){
  SDInput input;
  input.cut_size = 10000000;
  input.thread_count = 2;
  SDBuilder builder(SDBuilder::SDType::HRPDACBlocks, false, input);

  std::stringstream ss;

  std::vector<std::string> data_vec = {
    "string_A",
    "string_B",
    "string_C",
    "string_D",
    "string_E",
    "string_F",
    "string_G",
  };

  for(const auto &s : data_vec){
    ss << s << "\n";
  }

  auto sd = builder.build(ss);

  std::unordered_set<unsigned long> indexes;
  for(const auto &s: data_vec){
    auto *uchar_data = reinterpret_cast<unsigned char*>(const_cast<char *>((s.data())));
    indexes.insert(sd->locate(uchar_data, s.size() ));
  }

  ASSERT_EQ(indexes.size(), data_vec.size());
  
}
