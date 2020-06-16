//
// Created by Cristobal Miranda, 2020
//

#ifndef RDFCACHEK2_RADIXITERATORDICTSTRING_HPP
#define RDFCACHEK2_RADIXITERATORDICTSTRING_HPP

#include "RadixTree.hpp"
#include <iterators/IteratorDictString.h>

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
    return out;
  }
};

#endif // RDFCACHEK2_RADIXITERATORDICTSTRING_HPP
