//
// Created by cristobal on 26-10-21.
//

#include "LazyLoadSDIter.hpp"

bool LazyLoadSDIter::hasNext() {
    return next_available;
}

unsigned char *LazyLoadSDIter::next(uint *str_length) {
    return next_concrete(str_length);
}

LazyLoadSDIter::LazyLoadSDIter(I_IStream &istream) :
        istream(istream),
        next_available(false),
        last_value(nullptr),
        last_length(0) {
    last_value = next_concrete(&last_length);
}

// user is expected to manage this returned resource
unsigned char *LazyLoadSDIter::next_concrete(uint *str_length) {

    *str_length = last_length;
    auto out = last_value;

    std::string line;
    if(!std::getline(istream.get_stream(),line)){
        last_length = 0;
        next_available = false;
    }
    else{
        last_length = line.size();
        last_value = new unsigned char[last_length+1]{};
        strcpy(reinterpret_cast<char *>(last_value), line.data());
        next_available = true;
    }

    return out;
}
