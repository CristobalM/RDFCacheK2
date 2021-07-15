//
// Created by cristobal on 7/14/21.
//

#ifndef RDFCACHEK2_QUERYPROCHASHING_HPP
#define RDFCACHEK2_QUERYPROCHASHING_HPP

#include <cstddef>
#include <vector>
// http://www.isthe.com/chongo/tech/comp/fnv/index.html#public_domain, should
// be used on octects (1byte) instead of on 8 byte numbers
struct fnv_hash_64 {
  std::size_t operator()(const std::vector<unsigned long> &key) const;
};

#endif // RDFCACHEK2_QUERYPROCHASHING_HPP
