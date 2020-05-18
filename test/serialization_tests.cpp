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

  proto_msg::K2Tree k2tree_proto_rev;
  k2tree_proto_rev.ParseFromString(serialized);

  K2Tree k2tree_deserialized(k2tree_proto_rev);
}