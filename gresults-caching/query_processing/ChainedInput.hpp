
//
// Created by Cristobal Miranda, 2020
//

#ifndef RDFCACHEK2_CHAINEDINPUT_HPP
#define RDFCACHEK2_CHAINEDINPUT_HPP

#include <vector>

#include "K2TreeMixed.hpp"
#include "PredicatesCacheManager.hpp"
#include "ResultTable.hpp"
#include "Triple.hpp"

struct ChainedInput {
  ResultTable &result_table;
  const std::vector<Triple> &triples;
  const PredicatesCacheManager &cm;
  const std::vector<coord_t> &coord_types;
  ChainedInput(ResultTable &result_table, const std::vector<Triple> &triples,
               const PredicatesCacheManager &cm,
               const std::vector<coord_t> &coord_types);
};

#endif
