//
// Created by Cristobal Miranda, 2020
//

#ifndef RDFCACHEK2_UTILS_HPP
#define RDFCACHEK2_UTILS_HPP

#include <boost/endian/conversion.hpp>

template <class T> void native_to_big_arr(T array[], size_t array_sz) {
  for (size_t i = 0; i < array_sz; i++) {
    T value = array[i];
    boost::endian::native_to_big_inplace(value);
    array[i] = value;
  }
}

template <class T> void big_to_native_arr(T array[], size_t array_sz) {
  for (size_t i = 0; i < array_sz; i++) {
    T value = array[i];
    boost::endian::big_to_native_inplace(value);
    array[i] = value;
  }
}

#endif // RDFCACHEK2_UTILS_HPP
