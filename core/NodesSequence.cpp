//
// Created by cristobal on 20-11-21.
//

#include <stdexcept>
#include "NodesSequence.hpp"

long NodesSequence::get_id(long value) {
    auto it = std::lower_bound(values.begin(), values.end(), value);
    if(it == values.end()) return NOT_FOUND;
    if(*it != value) return NOT_FOUND;
    return it - values.begin();
}

long NodesSequence::get_value(long position) {
    if(position >= (long)values.size()) return NOT_FOUND;
    return values[position];
}

NodesSequence::NodesSequence(std::vector<long> &&values) : values(std::move(values)) {

}
