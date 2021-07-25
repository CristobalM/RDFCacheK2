//
// Created by cristobal on 7/14/21.
//

#include "QueryProcHashing.hpp"
std::size_t
fnv_hash_64::operator()(const std::vector<unsigned long> &key) const {
  unsigned long hash = 14695981039346656037UL;
  for (auto value : key) {
    hash *= 1099511628211;
    hash ^= value;
  }
  return static_cast<std::size_t>(hash);
}
