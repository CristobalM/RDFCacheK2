//
// Created by cristobal on 17-11-21.
//

#include <gtest/gtest.h>
#include "mock_structures/StringOStream.hpp"
#include "NodesSequence.hpp"

TEST(NodesMapSuite, CanGetNodesIdCorrectly){

    std::stringstream ss;

    std::vector<long> input_nums;

    for(long i = 0; i < 100000; i++){
        input_nums.push_back((i+1)*100);
    }

    NodesSequence nodes_sequence(std::move(input_nums));

    for(long i = 0; i < 100000; i++){
        auto value = (i+1)*100;
        auto id = nodes_sequence.get_id(value);
        ASSERT_EQ(id, i);

        auto extracted_value = nodes_sequence.get_value(i);
        ASSERT_EQ(extracted_value, value);
    }
}

TEST(NodesMapSuite, NotFoundFailGraciously){
    std::stringstream ss;

    std::vector<long> input_nums;

    for(long i = 0; i < 100000; i++){
        input_nums.push_back((i+1)*100);
    }

    NodesSequence nodes_sequence(std::move(input_nums));

    for(long i = 0; i < 100000; i++){
        auto value = (i+1)*100 + 1;
        auto id = nodes_sequence.get_id(value);
        ASSERT_EQ(id, NodesSequence::NOT_FOUND);
        auto extracted_value = nodes_sequence.get_value(i + 100000);
        ASSERT_EQ(extracted_value, NodesSequence::NOT_FOUND);
    }
}
