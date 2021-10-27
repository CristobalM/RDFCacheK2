//
// Created by cristobal on 26-10-21.
//

#ifndef RDFCACHEK2_SDBLOCKSWRAPPER_HPP
#define RDFCACHEK2_SDBLOCKSWRAPPER_HPP


#include <StringDictionary.h>
#include <iterators/IteratorDictStringPlain.h>
#include <StringDictionaryHASHRPDACBlocks.h>
#include "ItPlainWrapperWInfo.hpp"

class SDBlocksWrapper : public StringDictionary {

    std::unique_ptr<StringDictionaryHASHRPDACBlocks> internal_sd;
public:

    SDBlocksWrapper(ItPlainWrapperWInfo itw,
                    unsigned long len, int overhead,
                    unsigned long cut_size, int thread_count);

    unsigned long locate(uchar *str, uint strLen) override;

    uchar *extract(size_t id, uint *strLen) override;

    IteratorDictID *locatePrefix(uchar *str, uint strLen) override;

    IteratorDictID *locateSubstr(uchar *str, uint strLen) override;

    uint locateRank(uint rank) override;

    IteratorDictString *extractPrefix(uchar *str, uint strLen) override;

    IteratorDictString *extractSubstr(uchar *str, uint strLen) override;

    uchar *extractRank(uint rank, uint *strLen) override;

    IteratorDictString *extractTable() override;

    size_t getSize() override;

    void save(std::ostream &out) override;

};


#endif //RDFCACHEK2_SDBLOCKSWRAPPER_HPP
