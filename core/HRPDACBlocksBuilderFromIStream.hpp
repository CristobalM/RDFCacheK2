//
// Created by cristobal on 28-10-21.
//

#ifndef RDFCACHEK2_HRPDACBLOCKSBUILDERFROMISTREAM_HPP
#define RDFCACHEK2_HRPDACBLOCKSBUILDERFROMISTREAM_HPP


#include <I_IStream.hpp>
#include <StringDictionary.h>

class HRPDACBlocksBuilderFromIStream {
    I_IStream &input_stream;
public:
    explicit HRPDACBlocksBuilderFromIStream(I_IStream &input_stream);

    std::unique_ptr<StringDictionary> SDHRPDACBlocksWrapper;
};


#endif //RDFCACHEK2_HRPDACBLOCKSBUILDERFROMISTREAM_HPP
