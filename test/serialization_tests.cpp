//
// Created by Cristobal Miranda, 2020
//

#include <google/protobuf/message_lite.h>
#include <gtest/gtest.h>
#include <random>
#include <serialization_util.hpp>

TEST(u64_network_host, test_one) {
  std::random_device rd;

  std::mt19937_64 e2(rd());

  std::uniform_int_distribution<long long int> dist(
      std::llround(std::pow(2, 61)), std::llround(std::pow(2, 62)));

  for (int i = 0; i < 1'000'000; i++) {
    std::ostringstream oss;
    uint64_t val = dist(e2);
    write_u64(oss, val);
    auto stored = oss.str();
    std::istringstream iss(stored);
    auto result = read_u64(iss);
    ASSERT_EQ(val, result) << "Fails for val = " << val;
  }
}

int main(int argc, char **argv) {
  ::testing::InitGoogleTest(&argc, argv);
  int result = RUN_ALL_TESTS();
  google::protobuf::ShutdownProtobufLibrary();
  return result;
}
