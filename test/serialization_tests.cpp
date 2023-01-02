//
// Created by Cristobal Miranda, 2020
//

#include <google/protobuf/message_lite.h>
#include <gtest/gtest.h>
#include <random>
#include <serialization_util.hpp>
#include "custom_endian.hpp"
using namespace k2cache;
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

TEST(u64_network_host, endianness_from_host){
  auto v = 1ULL;
  auto nval = htobe64(v);
  ASSERT_EQ(nval, 1ULL << static_cast<uint64_t>(64-8));
  v = (1ULL << 8ULL)-1;
  auto shifted = v << (64 -8);
  nval = htobe64(v);
  ASSERT_EQ(nval, shifted);
}
TEST(u64_network_host, endianness_from_net){
  auto v = 1ULL;
  auto nval = be64toh(v);
  ASSERT_EQ(nval, 1ULL << static_cast<uint64_t>(64-8));
  v = (1ULL << 8ULL)-1;
  auto shifted = v << (64 -8);
  nval = be64toh(v);
  ASSERT_EQ(nval, shifted);
}

int main(int argc, char **argv) {
  ::testing::InitGoogleTest(&argc, argv);
  int result = RUN_ALL_TESTS();
  google::protobuf::ShutdownProtobufLibrary();
  return result;
}
