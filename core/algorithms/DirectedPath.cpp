//
// Created by cristobal on 12-12-22.
//

#include "DirectedPath.hpp"

namespace k2cache {

bool DirectedPath::operator==(const DirectedPath &other) const {
  if(&other == this) return true;
  if(other.edges.size() != edges.size()) return false;
  for(size_t i = 0; i < edges.size(); i++){
    if(!(edges[i] == other.edges[i])) return false;
  }
  return true;
}

std::size_t DirectedPath::hash() const {
  auto result = 0UL;
  auto cnt = 0UL;
  for (auto v : edges) {
    result ^= v.get_hash() * (31UL << (++cnt));
  }
  return result;
}
DirectedPath::DirectedPath(std::vector<RDFTriple> edges)
    : edges(std::move(edges)) {}
const std::vector<RDFTriple> &DirectedPath::get_vec() {
  return edges;
}

bool DirectedPath::operator<(const DirectedPath &other) const {
  const auto min_sz = std::min(edges.size(), other.edges.size());
  for(size_t i = 0; i < min_sz; i++){
    auto lhs = edges[i];
    auto rhs = other.edges[i];
    if(lhs == rhs){
      continue;
    }
    return lhs < rhs;
  }
  /**
   * if equal size, this is false, which is good
   * if less than other size, this is true, which is good
   * if greater than other size, this is false, which is good
   */
  return edges.size() < other.edges.size();
}

} // namespace k2cache