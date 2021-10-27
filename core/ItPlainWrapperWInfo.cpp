//
// Created by cristobal on 26-10-21.
//

#include "ItPlainWrapperWInfo.hpp"

ItPlainWrapperWInfo::ItPlainWrapperWInfo(IteratorDictStringPlain *it, size_t num_elements)
: it(it), num_elements(num_elements)
{

}

size_t ItPlainWrapperWInfo::get_num_elements() {
    return num_elements;
}

IteratorDictStringPlain *ItPlainWrapperWInfo::get_it() {
    return it;
}
