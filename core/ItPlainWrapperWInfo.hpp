//
// Created by cristobal on 26-10-21.
//

#ifndef RDFCACHEK2_ITPLAINWRAPPERWINFO_HPP
#define RDFCACHEK2_ITPLAINWRAPPERWINFO_HPP


#include <iterators/IteratorDictStringPlain.h>

class ItPlainWrapperWInfo {
    IteratorDictStringPlain *it;
    size_t num_elements;
public:
    ItPlainWrapperWInfo(IteratorDictStringPlain *it, size_t num_elements);
    size_t get_num_elements();
    IteratorDictStringPlain *get_it();
};


#endif //RDFCACHEK2_ITPLAINWRAPPERWINFO_HPP
