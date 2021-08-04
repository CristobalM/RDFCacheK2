#include <gtest/gtest.h>

#include <sstream>

#include <google/protobuf/message_lite.h>
#include <serialization_util.hpp>
#include <triple_external_sort.hpp>

struct Comparator {

  bool operator()(const TripleValue &lhs, const TripleValue &rhs) {
    return lhs.second < rhs.second;
  }
};

TEST(ExternalSortSuite, test_1) {
  std::string test_file = "test_file.bin";
  std::string test_out_file = "test_out_file.bin";

  std::ofstream ofs(test_file,
                    std::ios::out | std::ios::binary | std::ios::trunc);

  uint64_t size = 10'000'000;
  write_u64(ofs, size);
  for (uint64_t i = 0; i < size; i++) {
    TripleValue triple(size - i - 1, size - i - 1, size - i - 1);
    triple.write_to_file(ofs);
  }

  ofs.close();

  external_sort_triples(test_file, test_out_file, "./", 3, 10, 100'000'000,
                        8192, 10'000'000, Comparator());

  std::ifstream ifs(test_out_file, std::ios::in | std::ios::binary);

  FileData filedata;
  filedata.size = read_u64(ifs);
  for (uint64_t i = 0; i < filedata.size; i++) {
    auto triple = filedata.read_triple(ifs);
    ASSERT_EQ(triple.first, i);
    ASSERT_EQ(triple.second, i);
    ASSERT_EQ(triple.third, i);
  }

  ifs.close();

  remove(test_out_file.c_str());
  remove(test_file.c_str());
}

int main(int argc, char **argv) {
  ::testing::InitGoogleTest(&argc, argv);
  int result = RUN_ALL_TESTS();
  google::protobuf::ShutdownProtobufLibrary();
  return result;
}
