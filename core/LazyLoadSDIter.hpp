//
// Created by cristobal on 26-10-21.
//

#ifndef RDFCACHEK2_LAZYLOADSDITER_HPP
#define RDFCACHEK2_LAZYLOADSDITER_HPP


#include <iterators/IteratorDictString.h>
#include <I_IStream.hpp>

class LazyLoadSDIter : public IteratorDictString {
    I_IStream &istream;

    bool next_available;

    unsigned char *last_value;
    uint last_length;
public:

    explicit LazyLoadSDIter(I_IStream &istream);

    bool hasNext() override;

    unsigned char *next(uint *str_length) override;

private:
    unsigned char *next_concrete(uint *str_length);
};


#endif //RDFCACHEK2_LAZYLOADSDITER_HPP
