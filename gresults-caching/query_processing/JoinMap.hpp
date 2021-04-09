//
// Created by Cristobal Miranda, 2020
//

#ifndef RDFCACHEK2_JOINMAP_HPP
#define RDFCACHEK2_JOINMAP_HPP

#include <unordered_map>
#include <vector>

class JoinMap {

  std::unordered_map<std::vector<unsigned long>, std::vector<unsigned long>>
      inner_map;

public:
  JoinMap();
};

#endif