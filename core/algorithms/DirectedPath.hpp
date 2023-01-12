//
// Created by cristobal on 11-12-22.
//

#ifndef RDFCACHEK2_DIRECTEDPATH_HPP
#define RDFCACHEK2_DIRECTEDPATH_HPP

#include "k2tree/RDFTriple.hpp"
#include <cstddef>
#include <vector>

namespace k2cache {

class DirectedPath {
  std::vector<RDFTriple> edges;
public:

  explicit DirectedPath(std::vector<RDFTriple> edges);


  bool operator==(const DirectedPath &other) const;
  [[nodiscard]] std::size_t hash() const;
  const std::vector<RDFTriple> &get_vec();
  bool operator<(const DirectedPath &other) const;
};

}

namespace std {
template<>
struct hash<k2cache::DirectedPath>{
  std::size_t operator()(const k2cache::DirectedPath &dp ) const {
    return dp.hash();
  }
};
}

#endif // RDFCACHEK2_DIRECTEDPATH_HPP
