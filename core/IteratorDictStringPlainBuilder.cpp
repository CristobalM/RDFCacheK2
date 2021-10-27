//
// Created by cristobal on 26-10-21.
//

#include <iterators/IteratorDictStringPlain.h>
#include "IteratorDictStringPlainBuilder.hpp"

IteratorDictStringPlainBuilder::IteratorDictStringPlainBuilder(IteratorDictString &iterator, size_t buffer_size)
        : iterator(iterator), buffer_size(buffer_size), carry_string(nullptr), carry_size(0), has_carry_string(false) {
    data.reserve(buffer_size);
}

ItPlainWrapperWInfo IteratorDictStringPlainBuilder::next_it() {
    size_t accumulated_size = 0;

    size_t num_elements = 0;
    if (has_carry_string) {
        data.insert(data.end(), carry_string, carry_string + carry_size + 1);
        accumulated_size += (size_t) carry_size + 1;
        has_carry_string = false;
        carry_string = nullptr;
        carry_size = 0;
        num_elements++;
    }

    while (iterator.hasNext()) {
        uint next_string_size;
        auto *next_string = iterator.next(&next_string_size);
        auto next_size = accumulated_size + (size_t) (next_string_size) + 1;
        if (next_size > buffer_size) {
            carry_string = next_string;
            carry_size = next_string_size;
            has_carry_string = true;
            break;
        }
        // +1 adds null terminating character
        data.insert(data.end(), next_string, next_string + next_string_size + 1);
        accumulated_size = next_size;
        num_elements++;
    }

    auto *resulting_buffer = new unsigned char[accumulated_size];
    std::copy(data.begin(), data.begin() + (long) accumulated_size, resulting_buffer);
    auto *it = new IteratorDictStringPlain(resulting_buffer, accumulated_size);
    return {it, num_elements};
}

bool IteratorDictStringPlainBuilder::next_it_available() {
    return iterator.hasNext() || has_carry_string;
}
