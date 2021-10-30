//
// Created by cristobal on 26-10-21.
//

#ifndef RDFCACHEK2_SDHRPDACWRAPPER_HPP
#define RDFCACHEK2_SDHRPDACWRAPPER_HPP


#include <StringDictionary.h>
#include <iterators/IteratorDictStringPlain.h>
#include <StringDictionaryHASHRPDAC.h>
#include <I_IStream.hpp>
#include "ItPlainWrapperWInfo.hpp"

class SDHRPDACWrapper : public StringDictionary {

    std::unique_ptr<StringDictionaryHASHRPDAC> internal_sd;
public:

    SDHRPDACWrapper(ItPlainWrapperWInfo itw,
                    unsigned long len, int overhead);

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


#endif //RDFCACHEK2_SDHRPDACWRAPPER_HPP
