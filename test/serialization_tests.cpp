//
// Created by Cristobal Miranda, 2020
//

#include <K2Tree.hpp>
#include <graph_result.pb.h>
#include <gtest/gtest.h>

TEST(test_case_one, test_name_one) {
  K2Tree k2Tree(32, 1024);

  for (int i = 0; i < 100; i++) {
    for (int j = 0; j < 100; j++) {
      k2Tree.insert(i, j);
    }
  }
  proto_msg::K2Tree k2tree_proto;
  k2Tree.produce_proto(&k2tree_proto);
  auto serialized = k2tree_proto.SerializeAsString();

  std::cout << "serialized k2_tree size " << serialized.size() << std::endl;
  std::cout << "serialized k2 tree: \n" << serialized << std::endl;

  proto_msg::K2Tree k2tree_proto_rev;
  k2tree_proto_rev.ParseFromString(serialized);

  K2Tree k2tree_deserialized(k2tree_proto_rev);

  ASSERT_TRUE(k2Tree.same_as(k2tree_deserialized)) << "Trees not matching";

  for (int i = 0; i < 100; i++) {
    for (int j = 0; j < 100; j++) {
      ASSERT_TRUE(k2Tree.has(i, j)) << "normal doesn't have " << i << ", " << j;
      ASSERT_TRUE(k2tree_deserialized.has(i, j))
          << "deserialized doesn't have " << i << ", " << j;
    }
  }
}