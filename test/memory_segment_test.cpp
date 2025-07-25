//
// Created by cristobal on 20-08-21.
//

#include <MemoryManager.hpp>
#include <OutOfMemoryException.hpp>
#include <google/protobuf/message_lite.h>
#include <gtest/gtest.h>
using namespace k2cache;
TEST(memory_segment_test, test1) {
  const int total_values = 100;
  const size_t required_sz = sizeof(int) * total_values;
  auto *memory_segment =
      MemoryManager::instance().new_memory_segment(required_sz);
  auto req_integer = [memory_segment]() {
    return (int *)memory_segment->require_bytes(sizeof(int));
  };
  std::vector<int *> values;
  for (int i = 0; i < total_values; i++) {
    int *curr = req_integer();
    *curr = i;
    values.push_back(curr);
  }
  for (int i = 0; i < (int)values.size(); i++) {
    ASSERT_EQ(*values[i], i);
  }

  EXPECT_THROW(
      {
        try {
          req_integer();
        } catch (const OutOfMemoryException &e) {
          throw;
        }
      },
      OutOfMemoryException);
  for (int i = 0; i < (int)values.size(); i++) {
    auto *segment = MemoryManager::instance().find_segment(values[i]);
    ASSERT_EQ(segment, memory_segment) << "Failed at i = " << i;
  }
}

TEST(memory_segment_test, test2) {
  MemoryManager::instance().new_memory_segment(4 * 1000);
  auto *arr = k2tree_alloc_u32array(1000);
  k2tree_free_u32array(arr, 1000);
}

int main(int argc, char **argv) {
  ::testing::InitGoogleTest(&argc, argv);
  int result = RUN_ALL_TESTS();
  google::protobuf::ShutdownProtobufLibrary();
  return result;
}
