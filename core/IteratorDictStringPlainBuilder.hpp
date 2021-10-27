//
// Created by cristobal on 26-10-21.
//

#ifndef RDFCACHEK2_ITERATORDICTSTRINGPLAINBUILDER_HPP
#define RDFCACHEK2_ITERATORDICTSTRINGPLAINBUILDER_HPP

#include <vector>
#include "ItPlainWrapperWInfo.hpp"

class IteratorDictStringPlainBuilder  {
    std::vector<unsigned char> data;
    IteratorDictString &iterator;
    size_t buffer_size;

    unsigned char *carry_string;
    uint carry_size;
    bool has_carry_string;
public:
    IteratorDictStringPlainBuilder(IteratorDictString &iterator, size_t buffer_size);
    ItPlainWrapperWInfo next_it();
    bool next_it_available();
};


#endif //RDFCACHEK2_ITERATORDICTSTRINGPLAINBUILDER_HPP
