//
// Created by cristobal on 11-12-22.
//

#ifndef RDFCACHEK2_DIRECTEDPATH_HPP
#define RDFCACHEK2_DIRECTEDPATH_HPP

#include <cstddef>
#include <vector>

namespace k2cache {

class DirectedPath {
  std::vector<unsigned long> nodes;
public:


  bool operator==(DirectedPath &other);
  [[nodiscard]] std::size_t hash() const;
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
