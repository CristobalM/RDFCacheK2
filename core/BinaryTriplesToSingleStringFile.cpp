//
// Created by cristobal on 23-10-21.
//

#include <triple_external_sort.hpp>
#include "BinaryTriplesToSingleStringFile.hpp"

void BinaryTriplesToSingleStringFile::run(I_OStream &output_stream, I_IStream &input_stream) {
    auto &istream = input_stream.get_stream();
    auto &ostream = output_stream.get_stream();

    FileData file_data{};
    file_data.size = read_u64(istream);
    while(!file_data.finished()){
        auto triple = file_data.read_triple(istream);
        ostream
        << triple.first << "\n"
        << triple.second << "\n"
        << triple.third << "\n";
    }
}
