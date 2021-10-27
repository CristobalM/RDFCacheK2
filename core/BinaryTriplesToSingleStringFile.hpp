//
// Created by cristobal on 23-10-21.
//

#ifndef RDFCACHEK2_BINARYTRIPLESTOSINGLESTRINGFILE_HPP
#define RDFCACHEK2_BINARYTRIPLESTOSINGLESTRINGFILE_HPP


#include <I_OStream.hpp>
#include <I_IStream.hpp>

class BinaryTriplesToSingleStringFile {
public:

    static void run(I_OStream &output_stream, I_IStream &input_stream);

};


#endif //RDFCACHEK2_BINARYTRIPLESTOSINGLESTRINGFILE_HPP
