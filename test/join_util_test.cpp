#include <gtest/gtest.h>

#include <vector>

#include <MergeJoin.hpp>

TEST(join_util_test, can_merge_two_vectors) {
  std::vector<unsigned long> A = {3, 4, 6, 7, 8, 10};
  std::vector<unsigned long> B = {1, 2, 4, 5, 6, 8, 9, 10};
  auto C = MergeJoin::merge_vectors(A, B);

  ASSERT_EQ(C.size(), 4);

  ASSERT_EQ(C[0], 4);
  ASSERT_EQ(C[1], 6);
  ASSERT_EQ(C[2], 8);
  ASSERT_EQ(C[3], 10);
}
