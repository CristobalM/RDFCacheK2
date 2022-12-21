//
// Created by cristobal on 12-12-22.
//

#include "DirectedPath.hpp"

namespace k2cache {

bool DirectedPath::operator==(DirectedPath &other) {
  return edges == other.edges;
}

std::size_t DirectedPath::hash() const {
  auto result = 0UL;
  auto cnt = 0UL;
  for (auto v : edges) {
    result ^= v.get_hash() * (31UL << (++cnt));
  }
  return result;
}

} // namespace k2cache