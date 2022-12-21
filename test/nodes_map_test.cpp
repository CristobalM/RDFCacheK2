//
// Created by cristobal on 17-11-21.
//

#include "mock_structures/StringIStream.hpp"
#include "nodeids/NodesSequence.hpp"
#include "nodeids/node_ids_constants.hpp"
#include <gtest/gtest.h>
#include <serialization_util.hpp>

using namespace k2cache;

TEST(NodesMapSuite, CanGetNodesIdCorrectly) {
  std::stringstream ss;

  std::vector<long> input_nums;

  for (long i = 0; i < 100000; i++) {
    input_nums.push_back((i + 1) * 100);
  }

  NodesSequence nodes_sequence(std::move(input_nums));

  for (long i = 0; i < 100000; i++) {
    auto value = (i + 1) * 100;
    auto id = nodes_sequence.get_id(value);
    ASSERT_EQ(id, i);

    auto extracted_value = nodes_sequence.get_real_id(i);
    ASSERT_EQ(extracted_value, value);
  }
}

TEST(NodesMapSuite, NotFoundFailGraciously) {
  std::stringstream ss;

  std::vector<long> input_nums;

  for (long i = 0; i < 100000; i++) {
    input_nums.push_back((i + 1) * 100);
  }

  NodesSequence nodes_sequence(std::move(input_nums));

  for (long i = 0; i < 100000; i++) {
    auto value = (i + 1) * 100 + 1;
    auto id = nodes_sequence.get_id(value);
    ASSERT_EQ(id, NOT_FOUND_NODEID);
    auto extracted_value = nodes_sequence.get_real_id(i + 100000);
    ASSERT_EQ(extracted_value, NOT_FOUND_NODEID);
  }
}

TEST(NodesMapSuite, CanDeserialize) {
  std::stringstream ss;
  const long nodes_number = 1'000'000;
  write_u64(ss, nodes_number);
  for (unsigned long i = 0; i < nodes_number; i++) {
    write_u64(ss, i);
  }
  auto data = std::make_shared<std::string>(ss.str());
  StringIStream sis(data, std::ios::binary | std::ios::in);
  auto nodes_sequence = NodesSequence::from_input_stream(sis);
  for (long i = 0; i < nodes_number; i++) {
    auto value = nodes_sequence.get_real_id(i);
    ASSERT_EQ(value, i);
  }
}