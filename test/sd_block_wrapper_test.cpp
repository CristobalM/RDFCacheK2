
//
// Created by cristobal on 28-10-21.
//
#include <gtest/gtest.h>
#include <SDHRPDACWrapper.hpp>
#include <IteratorDictStringPlainBuilder.hpp>
#include <LazyLoadSDIter.hpp>
#include "mock_structures/StringIStream.hpp"
#include "mock_structures/StringIOStream.hpp"
#include "mock_structures/StringOStream.hpp"


TEST(SDBlocksWrapperTestSuite, test1){

    std::set<std::string> input_data = {
            "hello", "this", "is", "a", "test", "pl", "aixx"
    };

    std::string data;
    {
        StringOStream sostream(data, std::ios::out);
        for(auto &s: input_data){
            sostream.get_stream() << s << "\n";
        }
    }
    StringIStream sistream(data, std::ios::in);
    LazyLoadSDIter it(sistream);
    IteratorDictStringPlainBuilder builder(it, 1'000'000);
    auto itw = builder.next_it();
    SDHRPDACWrapper sdw(itw, itw.get_it()->size(), 25);
    auto it_table = std::unique_ptr<IteratorDictString>(sdw.extractTable());
    std::set<std::string> retrieved;
    while(it_table->hasNext()){
        uint length;
        uchar *raw_str = it_table->next(&length);
        retrieved.insert(std::string(reinterpret_cast<char *>(raw_str), length));
        delete [] raw_str;
    }

    ASSERT_EQ(retrieved, input_data);
}