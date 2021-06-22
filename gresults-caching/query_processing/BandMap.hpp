
//
// Created by Cristobal Miranda, 2020
//

#ifndef RDFCACHEK2_BANDMAP_HPP
#define RDFCACHEK2_BANDMAP_HPP

#include <unordered_map>
#include <vector>

#include "IBandMap.hpp"
#include <K2TreeMixed.hpp>

class BandMap : public IBandMap {

  using map_t = std::unordered_map<unsigned long, std::vector<unsigned long>>;

  map_t map;

  static const std::vector<unsigned long> empty_vec;

public:
  BandMap(const K2TreeMixed &k2tree, BType type);

  BandMap();
  const std::vector<unsigned long> &get_band(unsigned long key) override;
};

#endif