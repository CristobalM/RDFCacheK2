
//
// Created by Cristobal Miranda, 2020
//

#ifndef RDFCACHEK2_BANDMAP_LAZY_HPP
#define RDFCACHEK2_BANDMAP_LAZY_HPP

#include <set>
#include <unordered_map>
#include <vector>

#include "IBandMap.hpp"
#include <K2TreeMixed.hpp>

class BandMapLazy : public IBandMap {
private:
  using map_t = std::unordered_map<unsigned long, std::vector<unsigned long>>;

  map_t map;
  std::set<unsigned long> retrieved;

  const K2TreeMixed &k2tree;
  BType type;

  static const std::vector<unsigned long> empty_vec;

public:
  BandMapLazy(const K2TreeMixed &k2tree, BType type);

  const std::vector<unsigned long> &get_band(unsigned long key) override;
};

#endif