
//
// Created by Cristobal Miranda, 2020
//

#ifndef RDFCACHEK2_BANDMAP_HPP
#define RDFCACHEK2_BANDMAP_HPP

#include <unordered_map>
#include <vector>

#include <K2TreeMixed.hpp>

class BandMap {

  using map_t = std::unordered_map<unsigned long, std::vector<unsigned long>>;

  map_t map;

  static const std::vector<unsigned long> empty_vec;

public:
  enum BType { BY_COL = 0, BY_ROW = 1 };

  BandMap(const K2TreeMixed &k2tree, BType type);

  const std::vector<unsigned long> &operator[](unsigned long key);
  BandMap();
};

#endif