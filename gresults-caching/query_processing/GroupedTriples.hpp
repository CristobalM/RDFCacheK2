
//
// Created by Cristobal Miranda, 2020
//

#ifndef RDFCACHEK2_GROUPEDTRIPLES_HPP
#define RDFCACHEK2_GROUPEDTRIPLES_HPP

#include "KeyPairStr.hpp"
#include "Triple.hpp"
#include "hash_pair.hpp"
#include <string>
#include <unordered_map>
#include <vector>

struct GroupedTriples {
  std::unordered_map<std::string, std::vector<Triple>> one_var_groups;
  std::unordered_map<KeyPairStr, std::vector<Triple>, hash_pair> two_var_groups;

  GroupedTriples(
      std::unordered_map<std::string, std::vector<Triple>> &&one_var_groups,
      std::unordered_map<KeyPairStr, std::vector<Triple>, hash_pair>
          &&two_var_groups);
};

#endif