//
// Created by cristobal on 23-10-21.
//

#include <gtest/gtest.h>
#include <BinaryTriplesToSingleStringFile.hpp>
#include <serialization_util.hpp>
#include <triple_external_sort.hpp>
#include "mock_structures/StringIStream.hpp"
#include "mock_structures/StringOStream.hpp"

TEST(binary_triples_to_single_string_file_test, test1){
    const unsigned long total_triples = 100000;

    std::string created_data_binary;
    {
        StringOStream ostream_data_creator(created_data_binary, std::ios::out | std::ios::binary);
        write_u64(ostream_data_creator.get_stream(), total_triples);
        for(size_t i = 0; i < total_triples; i++){
            TripleValue triple_value(i, i+1, i+2);
            triple_value.write_to_file(ostream_data_creator.get_stream());
        }
    }

    StringIStream istream(created_data_binary, std::ios::in | std::ios::binary);

    std::string resulting_data_text;
    {
        StringOStream ostream(resulting_data_text, std::ios::out);
        BinaryTriplesToSingleStringFile::run(ostream, istream);
    }

    StringIStream check_input_streamer(resulting_data_text, std::ios::in);

    std::set<unsigned long> read_values;
    auto &in_mem_stream = check_input_streamer.get_stream();
    std::string line;
    while(std::getline(in_mem_stream, line)){
        auto value = std::stoul(line);
        read_values.insert(value);
    }

    std::set<unsigned long> expected_values;
    for(size_t i = 0; i < total_triples; i++){
        expected_values.insert(i);
        expected_values.insert(i+1);
        expected_values.insert(i+2);
    }

    ASSERT_EQ(expected_values, read_values);
}