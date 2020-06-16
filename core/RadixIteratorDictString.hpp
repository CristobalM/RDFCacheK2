//
// Created by Cristobal Miranda, 2020
//

#ifndef RDFCACHEK2_RADIXITERATORDICTSTRING_HPP
#define RDFCACHEK2_RADIXITERATORDICTSTRING_HPP

#include "RadixTree.hpp"
#include <iterators/IteratorDictString.h>
#include <string_view>

template <class RDataT = NoDataEntity>
class RadixIteratorDictString : public IteratorDictString {

  RadixTree<RDataT> &radix_tree;
  typename RadixTree<RDataT>::iterator it;
public:
  explicit RadixIteratorDictString(RadixTree<RDataT> &radix_tree)
      : radix_tree(radix_tree), it(radix_tree.begin()) {
  }

  bool hasNext() override {
    auto valid = it.is_valid();
    return valid;
  }

  unsigned char *next(uint *str_length) override {
    auto &rax_it = it.rax_iterator();
    auto *out = rax_it.key;
    *str_length = rax_it.key_len;
    auto *held_str = new unsigned char[*str_length];
    memcpy(held_str, out, *str_length);
    ++it;
    //std::cout << std::string_view(reinterpret_cast<const char *>(out), *str_length) << std::endl;
    return held_str;
  }
};

#endif // RDFCACHEK2_RADIXITERATORDICTSTRING_HPP
